      // Copyright 2014, Additive Regularization of Topic Models.

#include "artm/memcached_server.h"

#include "glog/logging.h"

#include "artm/helpers.h"

namespace artm {
namespace core {

MemcachedServer::~MemcachedServer() {
  application_.terminate();
  thread_.join();
}

MemcachedServer::MemcachedServer(int id, const std::string& endpoint)
    : id_(id), endpoint_(endpoint), application_(rpcz::application::options(1)), thread_() {
  boost::thread t(&MemcachedServer::ThreadFunction, this);
  thread_.swap(t);
}

void MemcachedServer::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "Memcached thread");
    rpcz::server server(application_);
    ::artm::memcached::MemcachedServiceImpl memcached_service_impl;
    server.register_service(&memcached_service_impl);
    server.bind(endpoint());
    application_.run();
  } catch(...) {
    LOG(FATAL) << "Fatal exception in MemcachedServer::ThreadFunction() function";
    return;
  }
}

}  // namespace core
}  // namespace artm
