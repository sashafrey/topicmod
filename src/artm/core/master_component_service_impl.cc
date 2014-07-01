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
#include "artm/core/instance.h"
#include "artm/core/merger.h"
#include "artm/core/batch_manager.h"
#include "artm/core/zmq_context.h"
#include "artm/core/generation.h"

namespace artm {
namespace core {

MasterComponentServiceImpl::MasterComponentServiceImpl(Instance* instance, NetworkClientCollection* clients)
    : instance_(instance), application_(), clients_(clients) {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));
}

void MasterComponentServiceImpl::UpdateModel(const ::artm::core::ModelIncrement& request,
                                       ::rpcz::reply< ::artm::core::Void> response) {
    instance_->merger_queue()->push(std::make_shared<::artm::core::ModelIncrement>(request));
  try {
    response.send(::artm::core::Void());
  } catch(...) {
    LOG(ERROR) << "Unable to send reply to UpdateModel.";
  }
}

void MasterComponentServiceImpl::RetrieveModel(const ::artm::core::String& request,
                                         ::rpcz::reply< ::artm::TopicModel> response) {
  ::artm::TopicModel topic_model;
  bool succeeded = instance_->merger()->RetrieveExternalTopicModel(request.value(), &topic_model);
  try {
    if (succeeded) {
      response.send(topic_model);
    } else {
      response.Error(0, "Model with requested ID was does not exist on server");
    }
  } catch(...) {
    LOG(ERROR) << "Unable to send reply to UpdateModel.";
  }
}

void MasterComponentServiceImpl::RequestBatches(const ::artm::core::Int& request,
                      ::rpcz::reply< ::artm::core::BatchIds> response) {
  BatchIds reply;
  for (int i = 0; i < request.value(); ++i) {
    boost::uuids::uuid uuid = instance_->batch_manager()->Next();
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

    instance_->batch_manager()->Done(uuid, ModelName());
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
      instance_->batch_manager()->Add(uuid);
    }
  }
}

void MasterComponentServiceImpl::WaitIdle() {
  // Wait for all nodes to process all the batches.
  for (;;) {
      if (instance_->batch_manager()->IsEverythingProcessed())
      break;

    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }

  // Ask all nodes to push their updates to topic model
  clients_->for_each_client([&](NodeControllerService_Stub& client) {
    Void response;
    try {
      client.ForcePushTopicModelIncrement(Void(), &response);
    } catch(...) {
      LOG(ERROR) << "Unable to force push topic model increment on one of clients";
    }
  });

  // Wait merger on master to process all model increments and set them as active topic model
  instance_->merger()->WaitIdle();
  instance_->merger()->ForcePushTopicModelIncrement();
  instance_->merger()->ForcePullTopicModel();

  // Ask all nodes to pull the new model
  clients_->for_each_client([&](NodeControllerService_Stub& client) {
    Void response;
    try {
      client.ForcePullTopicModel(Void(), &response);
    } catch(...) {
      LOG(ERROR) << "Unable to force pull topic model on one of clients";
    }
  });
}

}  // namespace core
}  // namespace artm
