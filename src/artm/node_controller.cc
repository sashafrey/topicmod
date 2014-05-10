// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/node_controller.h"
#include "artm/node_controller_service_impl.h"

#include "glog/logging.h"

#include "artm/exceptions.h"
#include "artm/helpers.h"

namespace artm {
namespace core {

NodeController::NodeController(int id, const NodeControllerConfig& config)
    : lock_(),
      node_controller_id_(id),
      config_(lock_, std::make_shared<NodeControllerConfig>(NodeControllerConfig(config))),
      service_endpoint_(nullptr),
      application_(),
      master_component_service_proxy_(nullptr)
{
  service_endpoint_.reset(new ServiceEndpoint(config.node_controller_endpoint()));

  master_component_service_proxy_.reset(
      new artm::core::MasterComponentService_Stub(
      application_.create_rpc_channel(config.master_component_endpoint()), true));

  ::artm::core::String request;
  ::artm::core::Void response;
  request.set_value(config.node_controller_endpoint());
  master_component_service_proxy_->ConnectClient(request, &response);
}

NodeController::~NodeController() {
  ::artm::core::String request;
  ::artm::core::Void response;
  request.set_value(config_.get()->node_controller_endpoint());
  master_component_service_proxy_->DisconnectClient(request, &response);

  if (service_endpoint_ != nullptr) {
    service_endpoint_.reset();
  }
}

int NodeController::id() const {
  return node_controller_id_;
}

NodeController::ServiceEndpoint::~ServiceEndpoint() {
  application_.terminate();
  thread_.join();
}

NodeController::ServiceEndpoint::ServiceEndpoint(const std::string& endpoint)
    : endpoint_(endpoint), application_(rpcz::application::options(1)), thread_() {
  boost::thread t(&NodeController::ServiceEndpoint::ThreadFunction, this);
  thread_.swap(t);
}

void NodeController::ServiceEndpoint::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "NodeController");
    rpcz::server server(application_);
    ::artm::core::NodeControllerServiceImpl node_controller_service_impl;
    server.register_service(&node_controller_service_impl);
    server.bind(endpoint());
    application_.run();
  } catch(...) {
    LOG(FATAL) << "Fatal exception in NodeController::ServiceEndpoint::ThreadFunction() function";
    return;
  }
}

}  // namespace core
}  // namespace artm
