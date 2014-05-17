// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/master_component_service_impl.h"

#include "boost/thread.hpp"
#include "glog/logging.h"

namespace artm {
namespace core {

MasterComponentServiceImpl::MasterComponentServiceImpl(
    ThreadSafeCollectionHolder<std::string, artm::core::NodeControllerService_Stub>* clients,
    zmq::context_t* zeromq_context)
    : lock_(), topic_model_(), application_(), clients_(clients) {
  rpcz::application::options options(1);
  options.zeromq_context = zeromq_context;
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
  if (clients_->has_key(request.value())) {
    response.Error(-1, "client with the same endpoint is already connected");
    return;
  }

  std::shared_ptr<NodeControllerService_Stub> client(
    new artm::core::NodeControllerService_Stub(
      application_->create_rpc_channel(request.value()), true));
  clients_->set(request.value(), client);
  response.send(artm::core::Void());
}

void MasterComponentServiceImpl::DisconnectClient(const ::artm::core::String& request,
                      ::rpcz::reply< ::artm::core::Void> response) {
  LOG(INFO) << "Receive disconnect request from client " << request.value();

  if (!clients_->has_key(request.value())) {
    response.Error(-1, "client with this endpoint is not connected");
    return;
  }

  std::string client_name = request.value();
  response.send(artm::core::Void());
  clients_->erase(client_name);
}

}  // namespace core
}  // namespace artm
