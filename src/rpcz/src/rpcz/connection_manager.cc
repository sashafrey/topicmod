// Copyright 2011 Google Inc. All Rights Reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: nadavs@google.com <Nadav Samet>

#include "rpcz/connection_manager.hpp"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include <map>
#include <ostream>
#if !defined(_WIN32) && !defined(_WIN64)
#include <pthread.h>
#endif
#include <sstream>
#include <stddef.h>
#include <string>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#else
#include "process.h"
#endif
#include <utility>
#include <vector>
#include "zmq.h"
#include "zmq.hpp"

#include "google/protobuf/stubs/common.h"
#include "rpcz/callback.hpp"
#include "rpcz/clock.hpp"
#include "rpcz/logging.hpp"
#include "rpcz/macros.hpp"
#include "rpcz/reactor.hpp"
#include "rpcz/zmq_utils.hpp"

namespace rpcz {
namespace {
const uint64 kLargePrime = (1ULL << 63) - 165;
const uint64 kGenerator = 2;

typedef uint64 event_id;

class event_id_generator {
 public:
  event_id_generator() {
#if !defined(_WIN32) && !defined(_WIN64)
    state_ = (reinterpret_cast<uint64>(this) << 32) + getpid();
#else
    state_ = (reinterpret_cast<uint64>(this) << 32) + _getpid();
#endif
  }

  event_id get_next() {
    state_ = (state_ * kGenerator) % kLargePrime;
    return state_;
  }

 private:
  uint64 state_;
  DISALLOW_COPY_AND_ASSIGN(event_id_generator);
};

// Command codes for internal process communication.
//
// Message sent from outside to the broker thread:
const char kRequest = 0x01;      // send request to a connected socket.
const char kConnect = 0x02;      // connect to a given endpoint.
const char kBind    = 0x03;      // bind to an endpoint.
const char kReply   = 0x04;      // reply to a request
const char kQuit    = 0x0f;      // Starts the quit second.

// Messages sent from the broker to a worker thread:
const char krunclosure        = 0x11;   // run a closure
const char krunserver_function = 0x12;   // Handle a request (a reply path
                                        // is given)
const char kInvokeclient_request_callback = 0x13;  // run a user supplied
                                                 // function that processes
                                                 // a reply from a remote
                                                 // server.
const char kWorkerQuit = 0x1f;          // Asks the worker to quit.

// Messages sent from a worker thread to the broker:
const char kReady = 0x21;        // Always the first message sent.
const char kWorkerDone = 0x22;   // Sent just before the worker quits.

std::string describe_command(char command) {
  switch (command) {
    case kRequest: return "kRequest";
    case kConnect: return "kConnect";
    case kBind: return "kBind";
    case kReply: return "kReply";
    case kQuit: return "kQuit";
    case krunclosure: return "krunclosure";
    case krunserver_function: return "krunserver_function";
    case kInvokeclient_request_callback: return "kInvokeclient_request_callback";
    case kWorkerQuit: return "kWorkerQuit";
    case kReady: return "kReady";
    case kWorkerDone: return "kWorkerDone";
  }
}

}  // unnamed namespace

struct remote_response_wrapper {
  int64 deadline_ms;
  uint64 start_time;
  connection_manager::client_request_callback callback;
};

void connection::send_request(
    message_vector& request,
    int64 deadline_ms,
    connection_manager::client_request_callback callback) {
  VLOG(1) << "connection::send_request()";
  remote_response_wrapper wrapper;
  wrapper.start_time = zclock_time();
  wrapper.deadline_ms = deadline_ms;
  wrapper.callback = callback;

  zmq::socket_t& socket = manager_->get_frontend_socket();
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_char(&socket, kRequest, ZMQ_SNDMORE);
  send_uint64(&socket, connection_id_, ZMQ_SNDMORE);
  send_object(&socket, wrapper, ZMQ_SNDMORE);
  write_vector_to_socket(&socket, request);
}

void client_connection::reply(message_vector* v) {
  VLOG(1) << "client_connection::reply()";
  zmq::socket_t& socket = manager_->get_frontend_socket();
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_char(&socket, kReply, ZMQ_SNDMORE);
  send_uint64(&socket, socket_id_, ZMQ_SNDMORE);
  send_string(&socket, sender_, ZMQ_SNDMORE);
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_string(&socket, event_id_, ZMQ_SNDMORE);
  write_vector_to_socket(&socket, *v);
}

void worker_thread(connection_manager* connection_manager,
                  zmq::context_t* context, std::string endpoint) {
  try {
    zmq::socket_t socket(*context, ZMQ_DEALER);
    std::string worker_id = Log::ptr_to_hex((void *)socket);
    LogModule worker_log_module("worker", worker_id);
    LOG(INFO) << connection_manager->log_module() << worker_log_module() << "is started";
    socket.connect(endpoint.c_str());
    send_empty_message(&socket, ZMQ_SNDMORE);
    send_string(&socket, worker_id, ZMQ_SNDMORE);
    send_char(&socket, kReady);
    bool should_quit = false;
    while (!should_quit) {
      message_iterator iter(socket);
      CHECK_EQ(0, iter.next().size());
      char command(interpret_message<char>(iter.next()));
      VLOG(1) << connection_manager->log_module() << worker_log_module() << describe_command(command);
      switch (command) {
        case kWorkerQuit: {
          should_quit = true;
          break;
        }
        case krunclosure: {
          interpret_message<closure*>(iter.next())->run();
          break;
        }
        case krunserver_function: {
          connection_manager::server_function sf =
              interpret_message<connection_manager::server_function>(iter.next());
          uint64 socket_id = interpret_message<uint64>(iter.next());
          std::string sender(message_to_string(iter.next()));
          if (iter.next().size() != 0) {
            break;
          }

          std::string event_id(message_to_string(iter.next()));

          sf(client_connection(connection_manager, socket_id, sender, event_id),
             iter);

          break;
        }
        case kInvokeclient_request_callback: {
          connection_manager::client_request_callback cb =
              interpret_message<connection_manager::client_request_callback>(
                  iter.next());
          connection_manager::status status = connection_manager::status(
              interpret_message<uint64>(iter.next()));

          cb(status, iter);

          break;
        }
      }
    }
    send_empty_message(&socket, ZMQ_SNDMORE);
    send_char(&socket, kWorkerDone);
    LOG(INFO) << connection_manager->log_module() << worker_log_module() << "is stopped";
  } catch(...) {
    LOG(FATAL) << "Fatal exception in worker_thread(connection_manager*) function";
    throw;
  }
}

class connection_manager_thread {
 public:
  connection_manager_thread(
      zmq::context_t* context,
      int nthreads,
      sync_event* ready_event,
      connection_manager* connection_manager,
      zmq::socket_t* frontend_socket) : 
    connection_manager_(connection_manager),
    context_(context),
    frontend_socket_(frontend_socket),
    current_worker_(0),
    log_module_("cmt", connection_manager){
      LOG(INFO) << log_module_() << "constructor...";

      wait_for_workers_ready_reply(nthreads);
      ready_event->signal();
      reactor_.add_socket(
          frontend_socket, new_permanent_callback(
              this, &connection_manager_thread::handle_frontend_socket,
              frontend_socket));

      LOG(INFO) << log_module_() << "constructor done";
    }

  void wait_for_workers_ready_reply(int nthreads) {
    for (int i = 0; i < nthreads; ++i) {
      message_iterator iter(*frontend_socket_);
      std::string sender = message_to_string(iter.next());
      CHECK_EQ(0, iter.next().size());
      std::string worker_id = message_to_string(iter.next());
      LOG(INFO) << log_module_() <<  "worker_id=" << worker_id
                <<" aka sender=" << Log::string_to_hex(sender) << " is ready.";
      char command(interpret_message<char>(iter.next()));
      CHECK_EQ(kReady, command) << "Got unexpected command " << (int)command;
      workers_.push_back(sender);
    }
  }

  static void run(zmq::context_t* context,
                  int nthreads,
                  sync_event* ready_event,
                  zmq::socket_t* frontend_socket,
                  connection_manager* connection_manager) {
    connection_manager_thread cmt(context, nthreads,
                                ready_event,
                                connection_manager, frontend_socket);
    try {
      LOG(INFO) << cmt.log_module() << "enter reactor loop";
      cmt.reactor_.loop();
      LOG(INFO) << cmt.log_module() << "leave reactor loop";
    } catch(...) {
      LOG(FATAL) << "Fatal exception in connection_manager_thread::run() function";
      throw;
    }
  }

  void handle_frontend_socket(zmq::socket_t* frontend_socket) {
    message_iterator iter(*frontend_socket);
    std::string sender = message_to_string(iter.next());
    CHECK_EQ(0, iter.next().size());
    char command(interpret_message<char>(iter.next()));

    VLOG(1) << log_module_() << "handle_frontend_socket("
            << "sender=" << Log::string_to_hex(sender) << ", "
            << "command=" << describe_command(command) << ")";

    switch (command) {
      case kQuit:
        // Ask the workers to quit. They'll in turn send kWorkerDone.
        for (int i = 0; i < workers_.size(); ++i) {
          send_string(frontend_socket_, workers_[i], ZMQ_SNDMORE);
          send_empty_message(frontend_socket_, ZMQ_SNDMORE);
          send_char(frontend_socket_, kWorkerQuit, 0);
        }
        break;
      case kConnect:
        handle_connect_command(sender, message_to_string(iter.next()));
        break;
      case kBind: {
        std::string endpoint(message_to_string(iter.next()));
        connection_manager::server_function sf(
            interpret_message<connection_manager::server_function>(
                iter.next()));
        handle_bind_command(sender, endpoint, sf);
        break;
      }
      case kRequest:
        send_request(iter);
        break;
      case kReply:
        send_reply(iter);
        break;
      case kReady:
        CHECK(false);
        break;
      case kWorkerDone:
        workers_.erase(std::remove(workers_.begin(), workers_.end(), sender));
        current_worker_ = 0;
        if (workers_.size() == 0) {
          // All workers are gone, time to quit.
          reactor_.set_should_quit();
        }
        break;
      case krunclosure:
        add_closure(interpret_message<closure*>(iter.next()));
        break;
    }
  }

  inline void begin_worker_command(char command) {
    VLOG(1) << log_module_() << "begin_worker_command()";
    send_string(frontend_socket_, workers_[current_worker_], ZMQ_SNDMORE);
    send_empty_message(frontend_socket_, ZMQ_SNDMORE);
    send_char(frontend_socket_, command, ZMQ_SNDMORE);
    ++current_worker_;
    if (current_worker_ == workers_.size()) {
      current_worker_ = 0;
    }
  }

  inline void add_closure(closure* closure) {
    LOG(INFO) << log_module_() << "add_closure()";
    begin_worker_command(krunclosure);
    send_pointer(frontend_socket_, closure, 0);
  }

  inline void handle_connect_command(const std::string& sender,
                                   const std::string& endpoint) {
    LOG(INFO) << log_module_() << "handle_connect_command()";
    zmq::socket_t* socket = new zmq::socket_t(*context_, ZMQ_DEALER);
    connections_.push_back(socket);
    int linger_ms = 0;
    socket->setsockopt(ZMQ_LINGER, &linger_ms, sizeof(linger_ms));
    socket->connect(endpoint.c_str());
    reactor_.add_socket(socket, new_permanent_callback(
            this, &connection_manager_thread::handle_client_socket,
            socket));

    send_string(frontend_socket_, sender, ZMQ_SNDMORE);
    send_empty_message(frontend_socket_, ZMQ_SNDMORE);
    send_uint64(frontend_socket_, connections_.size() - 1, 0);
  }

  inline void handle_bind_command(
      const std::string& sender,
      const std::string& endpoint,
      connection_manager::server_function server_function) {
    LOG(INFO) << log_module_() << "handle_bind_command()";
    zmq::socket_t* socket = new zmq::socket_t(*context_, ZMQ_ROUTER);
    int linger_ms = 0;
    socket->setsockopt(ZMQ_LINGER, &linger_ms, sizeof(linger_ms));
    socket->bind(endpoint.c_str());
    uint64 socket_id = server_sockets_.size();
    server_sockets_.push_back(socket);
    reactor_.add_socket(socket, new_permanent_callback(
            this, &connection_manager_thread::handle_server_socket,
            socket_id, server_function));

    send_string(frontend_socket_, sender, ZMQ_SNDMORE);
    send_empty_message(frontend_socket_, ZMQ_SNDMORE);
    send_empty_message(frontend_socket_, 0);
  }

  void handle_server_socket(uint64 socket_id,
                          connection_manager::server_function server_function) {
    VLOG(1) << log_module_() << "handle_server_socket()";
    message_iterator iter(*server_sockets_[socket_id]);
    begin_worker_command(krunserver_function);
    send_object(frontend_socket_, server_function, ZMQ_SNDMORE);
    send_uint64(frontend_socket_, socket_id, ZMQ_SNDMORE);
    forward_messages(iter, *frontend_socket_);
  }

  inline void send_request(message_iterator& iter) {
    VLOG(1) << log_module_() << "send_request()";
    uint64 connection_id = interpret_message<uint64>(iter.next());
    remote_response_wrapper remote_response_wrapper =
        interpret_message<rpcz::remote_response_wrapper>(iter.next());
    event_id event_id = event_id_generator_.get_next();
    remote_response_map_[event_id] = remote_response_wrapper.callback;
    if (remote_response_wrapper.deadline_ms != -1) {
      reactor_.run_closure_at(
          remote_response_wrapper.start_time +
              remote_response_wrapper.deadline_ms,
          new_callback(this, &connection_manager_thread::handle_timeout, event_id));
    }
    zmq::socket_t*& socket = connections_[connection_id];
    send_string(socket, "", ZMQ_SNDMORE);
    send_uint64(socket, event_id, ZMQ_SNDMORE);
    forward_messages(iter, *socket);
  }

  void handle_client_socket(zmq::socket_t* socket) {
    VLOG(1) << log_module_() << "handle_client_socket()";
    message_iterator iter(*socket);
    if (!iter.next().size() == 0) {
      return;
    }
    if (!iter.has_more()) {
      return;
    }
    event_id event_id(interpret_message<event_id>(iter.next()));
    remote_response_map::iterator response_iter = remote_response_map_.find(event_id);
    if (response_iter == remote_response_map_.end()) {
      return;
    }
    connection_manager::client_request_callback& callback = response_iter->second;
    begin_worker_command(kInvokeclient_request_callback);
    send_object(frontend_socket_, callback, ZMQ_SNDMORE);
    send_uint64(frontend_socket_, connection_manager::DONE, ZMQ_SNDMORE);
    forward_messages(iter, *frontend_socket_);
    remote_response_map_.erase(response_iter);
  }

  void handle_timeout(event_id event_id) {
    LOG(INFO) << log_module_() << "handle_timeout()";
    remote_response_map::iterator response_iter = remote_response_map_.find(event_id);
    if (response_iter == remote_response_map_.end()) {
      return;
    }
    connection_manager::client_request_callback& callback = response_iter->second;
    begin_worker_command(kInvokeclient_request_callback);
    send_object(frontend_socket_, callback, ZMQ_SNDMORE);
    send_uint64(frontend_socket_, connection_manager::DEADLINE_EXCEEDED, 0);
    remote_response_map_.erase(response_iter);
  }

  inline void send_reply(message_iterator& iter) {
    VLOG(1) << log_module_() << "send_reply()";
    uint64 socket_id = interpret_message<uint64>(iter.next());
    zmq::socket_t* socket = server_sockets_[socket_id];
    forward_messages(iter, *socket);
  }

  const std::string& log_module() const { return log_module_(); }

 private:
  typedef std::map<event_id, connection_manager::client_request_callback>
      remote_response_map;
  typedef std::map<uint64, event_id> deadline_map;
  connection_manager* connection_manager_;
  remote_response_map remote_response_map_;
  deadline_map deadline_map_;
  event_id_generator event_id_generator_;
  reactor reactor_;
  std::vector<zmq::socket_t*> connections_;
  std::vector<zmq::socket_t*> server_sockets_;
  zmq::context_t* context_;
  zmq::socket_t* frontend_socket_;
  std::vector<std::string> workers_;
  int current_worker_;
  LogModule log_module_;
};

connection_manager::connection_manager(zmq::context_t* context, int nthreads)
  : context_(context),
    frontend_endpoint_(
        "inproc://" + boost::lexical_cast<std::string>(this) + ".cm.frontend"),
    log_module_("connection_manager", this) {
  LOG(INFO) << log_module_() << "constructor...";
  LOG(INFO) << log_module_() << "frontend_endpoint_=" << frontend_endpoint_;

  zmq::socket_t* frontend_socket = new zmq::socket_t(*context, ZMQ_ROUTER);
  int linger_ms = 0;
  frontend_socket->setsockopt(ZMQ_LINGER, &linger_ms, sizeof(linger_ms));
  frontend_socket->bind(frontend_endpoint_.c_str());
  for (int i = 0; i < nthreads; ++i) {
    worker_threads_.add_thread(
        new boost::thread(&worker_thread, this, context, frontend_endpoint_));
  }
  sync_event event;
  broker_thread_ = boost::thread(&connection_manager_thread::run,
                                 context, nthreads, &event,
                                 frontend_socket, this);
  event.wait();
  LOG(INFO) << log_module_() << "constructor done.";
}

zmq::socket_t& connection_manager::get_frontend_socket() {
  zmq::socket_t* socket = socket_.get();
  if (socket == NULL) {
    socket = new zmq::socket_t(*context_, ZMQ_DEALER);
    int linger_ms = 0;
    socket->setsockopt(ZMQ_LINGER, &linger_ms, sizeof(linger_ms));
    socket->connect(frontend_endpoint_.c_str());
    socket_.reset(socket);
  }
  return *socket;
}

connection connection_manager::connect(const std::string& endpoint) {
  LOG(INFO) << log_module_() << "connect(endpoint=" << endpoint << ")";
  zmq::socket_t& socket = get_frontend_socket();
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_char(&socket, kConnect, ZMQ_SNDMORE);
  send_string(&socket, endpoint, 0);
  zmq::message_t msg;
  socket.recv(&msg);
  socket.recv(&msg);
  uint64 connection_id = interpret_message<uint64>(msg);
  return connection(this, connection_id);
}

void connection_manager::bind(const std::string& endpoint,
                             server_function function) {
  LOG(INFO) << log_module_() << "bing(" << endpoint << ")";
  zmq::socket_t& socket = get_frontend_socket();
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_char(&socket, kBind, ZMQ_SNDMORE);
  send_string(&socket, endpoint, ZMQ_SNDMORE);
  send_object(&socket, function, 0);
  zmq::message_t msg;
  socket.recv(&msg);
  socket.recv(&msg);
  return;
}

void connection_manager::add(closure* closure) {
  LOG(INFO) << log_module_() << "add closure";
  zmq::socket_t& socket = get_frontend_socket();
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_char(&socket, krunclosure, ZMQ_SNDMORE);
  send_pointer(&socket, closure, 0);
  return;
}
 
void connection_manager::run() {
  is_termating_.wait();
}

void connection_manager::terminate() {
  is_termating_.signal();
}

connection_manager::~connection_manager() {
  LOG(INFO) << log_module_() << "destructor...";
  zmq::socket_t& socket = get_frontend_socket();
  send_empty_message(&socket, ZMQ_SNDMORE);
  send_char(&socket, kQuit, 0);
  broker_thread_.join();
  worker_threads_.join_all();
  socket_.reset(NULL);
  LOG(INFO) << log_module_() << "destructor done.";
}
}  // namespace rpcz