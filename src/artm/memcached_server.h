// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MEMCACHED_SERVER_H_
#define SRC_ARTM_MEMCACHED_SERVER_H_

#include <string>

#include "boost/thread.hpp"
#include "boost/utility.hpp"

#include "artm/memcached_service_impl.h"
#include "artm/template_manager.h"

#include "rpcz/application.hpp"
#include "rpcz/rpc.hpp"
#include "rpcz/server.hpp"
#include "rpcz/service.hpp"

namespace artm {
namespace core {

class MemcachedServer : boost::noncopyable {
 public:
  std::string endpoint() const { return endpoint_; }
  int id() const { return id_; }

  ~MemcachedServer();

 private:
  friend class TemplateManager<MemcachedServer, std::string>;

  // All instances of MemcachedServiceInstance should be created via MemcachedServiceManager
  MemcachedServer(int id, const std::string& endpoint);

  std::string endpoint_;
  int id_;

  // Keep all threads at the end of class members
  // (because the order of class members defines initialization order;
  // everything else should be initialized before creating threads).
  boost::thread thread_;

  void ThreadFunction();
};

typedef TemplateManager<MemcachedServer, std::string> MemcachedServerManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_MEMCACHED_SERVER_H_
