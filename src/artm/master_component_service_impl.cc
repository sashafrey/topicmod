// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/master_component_service_impl.h"

#include "boost/thread.hpp"

namespace artm {
namespace core {

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
}

void MasterComponentServiceImpl::DisconnectClient(const ::artm::core::String& request,
                      ::rpcz::reply< ::artm::core::Void> response) {
}

}  // namespace core
}  // namespace artm
