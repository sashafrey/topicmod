// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/master_component_service_impl.h"

#include "boost/thread.hpp"
#include "glog/logging.h"

#include "artm/master_component.h"
#include "artm/zmq_context.h"

namespace artm {
namespace core {

MasterComponentServiceImpl::MasterComponentServiceImpl(
    NetworkClientCollection* clients)
    : lock_(), topic_model_(), application_(), clients_(clients) {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));
}

void MasterComponentServiceImpl::UpdateModel(const ::artm::core::ModelIncrement& request,
                                       ::rpcz::reply< ::artm::TopicModel> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  auto iter = topic_model_.find(request.model_id());
  if (iter == topic_model_.end()) {
    topic_model_.insert(std::make_pair(
      request.model_id(), std::make_shared<::artm::core::TopicModel>(request)));
    iter = topic_model_.find(request.model_id());
  } else {
    iter->second->ApplyDiff(request);
  }

  ::artm::TopicModel topic_model;
  iter->second->RetrieveExternalTopicModel(&topic_model);
  response.send(topic_model);
}

void MasterComponentServiceImpl::RetrieveModel(const ::artm::core::String& request,
                                         ::rpcz::reply< ::artm::TopicModel> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  auto iter = topic_model_.find(request.value());
  if (iter == topic_model_.end()) {
    response.Error(0, "Model with requested ID was does not exist on server");
  } else {
    ::artm::TopicModel topic_model;
    iter->second->RetrieveExternalTopicModel(&topic_model);
    response.send(topic_model);
  }
}

void MasterComponentServiceImpl::RequestBatches(const ::artm::core::Int& request,
                      ::rpcz::reply< ::artm::core::BatchIds> response) {
}

void MasterComponentServiceImpl::ReportBatches(const ::artm::core::BatchIds& request,
                      ::rpcz::reply< ::artm::core::Void> response) {
}

void MasterComponentServiceImpl::ConnectClient(const ::artm::core::String& request,
                      ::rpcz::reply< ::artm::core::Void> response) {
  LOG(INFO) << "Receive connect request from client " << request.value();
  bool success = clients_->ConnectClient(request.value(), application_.get());
  if (success) {
    response.send(artm::core::Void());
  } else {
    response.Error(-1, "client with the same endpoint is already connected");
  }
}

void MasterComponentServiceImpl::DisconnectClient(const ::artm::core::String& request,
                      ::rpcz::reply< ::artm::core::Void> response) {
  LOG(INFO) << "Receive disconnect request from client " << request.value();
  bool success = clients_->DisconnectClient(request.value());

  if (success) {
    response.send(artm::core::Void());
  } else {
    response.Error(-1, "client with this endpoint is not connected");
  }
}

}  // namespace core
}  // namespace artm
