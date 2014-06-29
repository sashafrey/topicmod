// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/master_component_service_impl.h"

#include <string>

#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/utility.hpp"
#include "glog/logging.h"

#include "rpcz/application.hpp"

#include "artm/core/master_component.h"
#include "artm/core/zmq_context.h"
#include "artm/core/generation.h"

namespace artm {
namespace core {

MasterComponentServiceImpl::MasterComponentServiceImpl(NetworkClientCollection* clients)
    : batch_manager_(), topic_model_(), application_(), clients_(clients) {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));
}

void MasterComponentServiceImpl::UpdateModel(const ::artm::core::ModelIncrement& request,
                                       ::rpcz::reply< ::artm::core::Void> response) {
  auto ttm = topic_model_.get(request.model_name());
  if (ttm == nullptr) {
    ttm = std::make_shared<::artm::core::TopicModel>(request);
    topic_model_.set(request.model_name(), ttm);
  } else {
    ttm->ApplyDiff(request);
  }

  response.send(::artm::core::Void());
}

void MasterComponentServiceImpl::RetrieveModel(const ::artm::core::String& request,
                                         ::rpcz::reply< ::artm::TopicModel> response) {
  auto ttm = topic_model_.get(request.value());
  if (ttm == nullptr) {
    response.Error(0, "Model with requested ID was does not exist on server");
  } else {
    ::artm::TopicModel topic_model;
    ttm->RetrieveExternalTopicModel(&topic_model);
    response.send(topic_model);
  }
}

void MasterComponentServiceImpl::RequestBatches(const ::artm::core::Int& request,
                      ::rpcz::reply< ::artm::core::BatchIds> response) {
  BatchIds reply;
  for (int i = 0; i < request.value(); ++i) {
    boost::uuids::uuid uuid = batch_manager_.Next();
    if (uuid.is_nil()) {
      break;
    }

    reply.add_batch_id(boost::lexical_cast<std::string>(uuid));
  }

  try {
    response.send(reply);
  } catch(...) {
    LOG(ERROR) << "Unable to send reply to RequestBatches.";
  }
}

void MasterComponentServiceImpl::ReportBatches(const ::artm::core::BatchIds& request,
                      ::rpcz::reply< ::artm::core::Void> response) {
  for (int i = 0; i < request.batch_id_size(); ++i) {
    boost::uuids::uuid uuid = boost::uuids::string_generator()(request.batch_id(i));
    if (uuid.is_nil()) {
      LOG(ERROR) << "Unable to convert " << request.batch_id(i) << " to uuid.";
      continue;
    }

    batch_manager_.Done(uuid);
  }

  try {
    response.send(Void());
  } catch(...) {
    LOG(ERROR) << "Unable to send reply to ReportBatches.";
  }
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

void MasterComponentServiceImpl::InvokeIteration(int iterations_count, std::string disk_path) {
  auto uuids = Generation::ListAllBatches(disk_path);
  for (int iter = 0; iter < iterations_count; ++iter) {
    for (auto &uuid : uuids) {
      batch_manager_.Add(uuid);
    }
  }
}

void MasterComponentServiceImpl::WaitIdle() {
  for (;;) {
    if (batch_manager_.IsEverythingProcessed())
      break;

    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }

  clients_->for_each_client([&](NodeControllerService_Stub& client) {
    Void response;
    try {
      client.ForcePushTopicModelIncrement(Void(), &response);
    } catch(...) {
      LOG(ERROR) << "Unable to force push topic model increment on one of clients";
    }
  });

  clients_->for_each_client([&](NodeControllerService_Stub& client) {
    Void response;
    try {
      client.ForcePullTopicModel(Void(), &response);
    } catch(...) {
      LOG(ERROR) << "Unable to force pull topic model on one of clients";
    }
  });
}

bool MasterComponentServiceImpl::RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) {
  auto ttm = topic_model_.get(model_name);
  if (ttm == nullptr) return false;
  ttm->RetrieveExternalTopicModel(topic_model);
  return true;
}

}  // namespace core
}  // namespace artm
