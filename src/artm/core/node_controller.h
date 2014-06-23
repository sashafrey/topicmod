// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_NODE_CONTROLLER_H_
#define SRC_ARTM_CORE_NODE_CONTROLLER_H_

#include <string>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "rpcz/application.hpp"
#include "rpcz/rpc.hpp"
#include "rpcz/server.hpp"
#include "rpcz/service.hpp"

#include "artm/messages.pb.h"
#include "artm/internals.pb.h"

#include "artm/core/template_manager.h"
#include "artm/core/thread_safe_holder.h"
#include "artm/core/internals.rpcz.h"

namespace artm {
namespace core {

class NodeController : boost::noncopyable {
 public:
  ~NodeController();
  int id() const;

 private:
  class ServiceEndpoint : boost::noncopyable {
   public:
    explicit ServiceEndpoint(const std::string& endpoint);
    ~ServiceEndpoint();
    std::string endpoint() const { return endpoint_; }

   private:
    std::string endpoint_;
    std::unique_ptr<rpcz::application> application_;

    // Keep all threads at the end of class members
    // (because the order of class members defines initialization order;
    // everything else should be initialized before creating threads).
    boost::thread thread_;

    void ThreadFunction();
  };

  friend class TemplateManager<NodeController, NodeControllerConfig>;

  // All node controllers must be created via TemplateManager.
  NodeController(int id, const NodeControllerConfig& config);

  mutable boost::mutex lock_;
  int node_controller_id_;
  ThreadSafeHolder<NodeControllerConfig> config_;

  std::shared_ptr<ServiceEndpoint> service_endpoint_;

  std::unique_ptr<rpcz::application> application_;
  std::shared_ptr<artm::core::MasterComponentService_Stub> master_component_service_proxy_;
};

typedef TemplateManager<NodeController, NodeControllerConfig> NodeControllerManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_NODE_CONTROLLER_H_
