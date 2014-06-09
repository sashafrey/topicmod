// Copyright 2014, Additive Regularization of Topic Models.

#include <string>

#include "artm/core/instance.h"

#include "boost/bind.hpp"

#include "artm/core/common.h"
#include "artm/core/exceptions.h"
#include "artm/core/processor.h"
#include "artm/core/merger.h"
#include "artm/core/template_manager.h"
#include "artm/core/topic_model.h"
#include "artm/core/zmq_context.h"
#include "artm/core/instance_schema.h"

#include "artm/regularizer_interface.h"
#include "artm/regularizer_sandbox/dirichlet_theta.h"
#include "artm/regularizer_sandbox/dirichlet_phi.h"
#include "artm/regularizer_sandbox/smooth_sparse_theta.h"
#include "artm/regularizer_sandbox/smooth_sparse_phi.h"

#define CREATE_OR_RECONFIGURE_REGULARIZER(ConfigType, RegularizerType) {                  \
  ConfigType regularizer_config;                                                          \
  if (!regularizer_config.ParseFromArray(config_blob.c_str(), config_blob.length())) {    \
    BOOST_THROW_EXCEPTION(SerializationException("Unable to parse regularizer config"));  \
  }                                                                                       \
  regularizer.reset(new RegularizerType(regularizer_config));                 \
}

namespace artm {
namespace core {

Instance::Instance(int id, const InstanceConfig& config)
    : lock_(),
      instance_id_(id),
      schema_(lock_, std::make_shared<InstanceSchema>(InstanceSchema(config))),
      application_(nullptr),
      master_component_service_proxy_(lock_, nullptr),
      processor_queue_lock_(),
      processor_queue_(),
      merger_queue_lock_(),
      merger_queue_(),
      merger_(),
      processors_() {
  merger_.reset(new Merger(
    &merger_queue_lock_, &merger_queue_, &schema_, &master_component_service_proxy_));
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));
  Reconfigure(config);
}

Instance::~Instance() {}

void Instance::ReconfigureModel(const ModelConfig& config) {
  merger_->UpdateModel(config);

  auto new_schema = schema_.get_copy();
  new_schema->set_model_config(config.name(), std::make_shared<const ModelConfig>(config));
  schema_.set(new_schema);
}

void Instance::DisposeModel(ModelName model_name) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_model_config(model_name);
  schema_.set(new_schema);

  merger_->DisposeModel(model_name);
}

void Instance::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  std::string regularizer_name = config.name();
  artm::RegularizerConfig_Type regularizer_type = config.type();
  std::string config_blob = config.config();

  std::shared_ptr<artm::RegularizerInterface> regularizer;

  // add here new case if adding new regularizer
  switch (regularizer_type) {
  case artm::RegularizerConfig_Type_DirichletTheta: {
      CREATE_OR_RECONFIGURE_REGULARIZER(::artm::DirichletThetaConfig,
                                        ::artm::regularizer_sandbox::DirichletTheta);
      break;
    }

    case artm::RegularizerConfig_Type_DirichletPhi: {
      CREATE_OR_RECONFIGURE_REGULARIZER(::artm::DirichletPhiConfig,
                                        ::artm::regularizer_sandbox::DirichletPhi);
      break;
    }

    case artm::RegularizerConfig_Type_SmoothSparseTheta: {
      CREATE_OR_RECONFIGURE_REGULARIZER(::artm::SmoothSparseThetaConfig,
                                        ::artm::regularizer_sandbox::SmoothSparseTheta);
      break;
    }

    case artm::RegularizerConfig_Type_SmoothSparsePhi: {
      CREATE_OR_RECONFIGURE_REGULARIZER(::artm::SmoothSparsePhiConfig,
                                        ::artm::regularizer_sandbox::SmoothSparsePhi);
      break;
    }

    default:
      BOOST_THROW_EXCEPTION(SerializationException("Unable to parse regularizer config"));
  }

  auto new_schema = schema_.get_copy();
  new_schema->set_regularizer(config.name(), regularizer);
  schema_.set(new_schema);
}

void Instance::DisposeRegularizer(const std::string& name) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_regularizer(name);
  schema_.set(new_schema);
}

void Instance::ForceResetScores(ModelName model_name) {
  merger_->ForceResetScores(model_name);
}

void Instance::ForceSyncWithMemcached(ModelName model_name) {
  merger_->ForceSyncWithMemcached(model_name);
}

void Instance::InvokePhiRegularizers() {
  merger_->InvokePhiRegularizers();
}

void Instance::Reconfigure(const InstanceConfig& config) {
  auto new_schema = schema_.get_copy();
  new_schema->set_instance_config(config);
  schema_.set(new_schema);

  // Adjust size of processors_; cast size to int to avoid compiler warning.
  while (static_cast<int>(processors_.size()) > config.processors_count()) processors_.pop_back();
  while (static_cast<int>(processors_.size()) < config.processors_count()) {
    processors_.push_back(
      std::shared_ptr<Processor>(new Processor(
        &processor_queue_lock_,
        &processor_queue_,
        &merger_queue_lock_,
        &merger_queue_,
        *merger_,
        schema_)));
  }

  // Recreate master_component_service_proxy_;
  if (config.has_master_component_endpoint()) {
    std::shared_ptr<artm::core::MasterComponentService_Stub> new_ptr(
      new artm::core::MasterComponentService_Stub(
        application_->create_rpc_channel(config.master_component_endpoint()), true));
    master_component_service_proxy_.set(new_ptr);
  } else {
    master_component_service_proxy_.set(nullptr);
  }
}

bool Instance::RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) {
  std::shared_ptr<const ::artm::core::TopicModel> ttm = merger_->GetLatestTopicModel(model_name);
  if (ttm == nullptr) return false;
  ttm->RetrieveExternalTopicModel(topic_model);
  return true;
}

int Instance::processor_queue_size() const {
  boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
  return processor_queue_.size();
}

void Instance::AddBatchIntoProcessorQueue(std::shared_ptr<const ProcessorInput> input) {
  boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
  processor_queue_.push(input);
}

}  // namespace core
}  // namespace artm
