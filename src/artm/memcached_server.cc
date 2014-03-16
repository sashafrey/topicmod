      // Copyright 2014, Additive Regularization of Topic Models.

#include "artm/memcached_server.h"
#include "artm/helpers.h"

namespace artm {
namespace core {

MemcachedServer::~MemcachedServer() {
  if (thread_.joinable()) {
    thread_.interrupt();
    thread_.join();
  }
}

MemcachedServer::MemcachedServer(int id, const std::string& endpoint)
    : id_(id), endpoint_(endpoint), thread_() {
  boost::thread t(&MemcachedServer::ThreadFunction, this);
  thread_.swap(t);
}

void MemcachedServer::ThreadFunction() {
  SetThreadName(-1, "Memcached thread");
  rpcz::application application;
  rpcz::server server(application);
  ::artm::memcached::MemcachedServiceImpl memcached_service_impl;
  server.register_service(&memcached_service_impl);
  server.bind(endpoint());
  application.run();
}

}  // namespace core
}  // namespace artm
