// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/memcached_service_impl.h"

namespace artm {
namespace memcached {

void MemcachedServiceImpl::UpdateModel(const ::artm::core::ModelIncrement& request,
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
void MemcachedServiceImpl::RetrieveModel(const ::artm::memcached::ModelId& request,
                                         ::rpcz::reply< ::artm::TopicModel> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  auto iter = topic_model_.find(request.model_id());
  if (iter == topic_model_.end()) {
    response.Error(0, "Model with requested ID was does not exist on server");
  } else {
    ::artm::TopicModel topic_model;
    iter->second->RetrieveExternalTopicModel(&topic_model);
    response.send(topic_model);
  }
}

}  // namespace memcached
}  // namespace artm
