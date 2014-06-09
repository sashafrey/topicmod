// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/node_controller.h"

#include "glog/logging.h"

#include "artm/core/node_controller_service_impl.h"
#include "artm/core/exceptions.h"
#include "artm/core/helpers.h"
#include "artm/core/zmq_context.h"

namespace artm {
namespace core {

NodeController::NodeController(int id, const NodeControllerConfig& config)
    : lock_(),
      node_controller_id_(id),
      config_(lock_, std::make_shared<NodeControllerConfig>(NodeControllerConfig(config))),
      service_endpoint_(nullptr),
      application_(nullptr),
      master_component_service_proxy_(nullptr) {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));

  service_endpoint_.reset(new ServiceEndpoint(config.node_controller_create_endpoint()));

  LOG(INFO) << "Connecting node " << config.node_controller_connect_endpoint()
            << " to master " << config.master_component_connect_endpoint();

  master_component_service_proxy_.reset(
      new artm::core::MasterComponentService_Stub(
      application_->create_rpc_channel(config.master_component_connect_endpoint()), true));

  ::artm::core::String request;
  ::artm::core::Void response;
  request.set_value(config.node_controller_connect_endpoint());
  master_component_service_proxy_->ConnectClient(request, &response);
}

NodeController::~NodeController() {
  auto config = config_.get();
  LOG(INFO) << "Disconnecting node " << config->node_controller_connect_endpoint()
            << " from master " << config->master_component_connect_endpoint();

  ::artm::core::String request;
  ::artm::core::Void response;
  request.set_value(config->node_controller_connect_endpoint());
  try {
    master_component_service_proxy_->DisconnectClient(request, &response);
  } catch(...) {
    LOG(ERROR) << "Unable to send disconnect message to master node.";
  }

  if (service_endpoint_ != nullptr) {
    service_endpoint_.reset();
  }
}

int NodeController::id() const {
  return node_controller_id_;
}

NodeController::ServiceEndpoint::~ServiceEndpoint() {
  application_->terminate();
  thread_.join();
}

NodeController::ServiceEndpoint::ServiceEndpoint(const std::string& endpoint)
    : endpoint_(endpoint), application_(nullptr), thread_() {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));
  boost::thread t(&NodeController::ServiceEndpoint::ThreadFunction, this);
  thread_.swap(t);
}

void NodeController::ServiceEndpoint::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "NodeController");
    LOG(INFO) << "Establishing NodeControllerService on " << endpoint();
    rpcz::server server(*application_);
    ::artm::core::NodeControllerServiceImpl node_controller_service_impl;
    server.register_service(&node_controller_service_impl);
    server.bind(endpoint());
    application_->run();
    LOG(INFO) << "NodeControllerService on " << endpoint() << " is stopped.";
  } catch(...) {
    LOG(FATAL) << "Fatal exception in NodeController::ServiceEndpoint::ThreadFunction() function";
    return;
  }
}

}  // namespace core
}  // namespace artm
