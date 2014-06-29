// Copyright 2014, Additive Regularization of Topic Models.

#include <string>
#include <utility>

#include "artm/core/instance.h"

#include "boost/bind.hpp"

#include "artm/core/common.h"
#include "artm/core/data_loader.h"
#include "artm/core/dictionary.h"
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
  regularizer.reset(new RegularizerType(regularizer_config));                             \
}                                                                                         \

namespace artm {
namespace core {

Instance::Instance(const MasterComponentConfig& config)
    : is_configured_(false),
      schema_(std::make_shared<InstanceSchema>(InstanceSchema(config))),
      application_(nullptr),
      master_component_service_proxy_(nullptr),
      processor_queue_(),
      merger_queue_(),
      merger_(),
      processors_(),
      dictionaries_(),
      local_data_loader_(nullptr),
      remote_data_loader_(nullptr),
      data_loader_(nullptr) {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));

  Reconfigure(config);
}

Instance::~Instance() {}

void Instance::CreateOrReconfigureModel(const ModelConfig& config) {
  merger_->CreateOrReconfigureModel(config);

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

  std::string config_blob;  // Used by CREATE_OR_RECONFIGURE_REGULARIZER
  if (config.has_config()) {
    config_blob = config.config();
  }

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

  regularizer->set_dictionaries(&dictionaries_);
  auto new_schema = schema_.get_copy();
  new_schema->set_regularizer(regularizer_name, regularizer);
  schema_.set(new_schema);
}

void Instance::DisposeRegularizer(const std::string& name) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_regularizer(name);
  schema_.set(new_schema);
}

void Instance::CreateOrReconfigureDictionary(const DictionaryConfig& config) {
  auto dictionary = std::make_shared<DictionaryMap>();
  for (int index = 0; index < config.entry_size(); ++index) {
    const ::artm::DictionaryEntry& entry = config.entry(index);
    dictionary->insert(std::pair<std::string, DictionaryEntry>(entry.key_token(), entry));
  }

  dictionaries_.set(config.name(), dictionary);
}

void Instance::DisposeDictionary(const std::string& name) {
  dictionaries_.erase(name);
}

void Instance::ForceResetScores(ModelName model_name) {
  merger_->ForceResetScores(model_name);
}

void Instance::ForcePullTopicModel() {
  merger_->ForcePullTopicModel();
}

void Instance::ForcePushTopicModelIncrement() {
  merger_->ForcePushTopicModelIncrement();
}

void Instance::InvokePhiRegularizers() {
  merger_->InvokePhiRegularizers();
}

void Instance::OverwriteTopicModel(const ::artm::TopicModel& topic_model) {
  merger_->OverwriteTopicModel(topic_model);
}

void Instance::Reconfigure(const MasterComponentConfig& config) {
  MasterComponentConfig old_config = schema_.get()->config();

  auto new_schema = schema_.get_copy();
  new_schema->set_config(config);
  schema_.set(new_schema);

  if (!is_configured_) {
    // First reconfiguration.

    // Recreate master_component_service_proxy_;
    if (config.modus_operandi() == MasterComponentConfig_ModusOperandi_Network) {
      master_component_service_proxy_.reset(
        new artm::core::MasterComponentService_Stub(
          application_->create_rpc_channel(config.master_component_connect_endpoint()), true));
    }

    // Reconfigure local/remote data loader
    if (config.modus_operandi() == MasterComponentConfig_ModusOperandi_Network) {
      remote_data_loader_.reset(new RemoteDataLoader(this));
      data_loader_ = remote_data_loader_.get();
    } else {
      local_data_loader_.reset(new LocalDataLoader(this));
      data_loader_ = local_data_loader_.get();
    }

    merger_.reset(new Merger(&merger_queue_, &schema_,
                             master_component_service_proxy_.get(), data_loader_));

    is_configured_  = true;
  } else {
    // Second and subsequent reconfiguration - some restrictions apply
    if (old_config.master_component_connect_endpoint() !=
        config.master_component_connect_endpoint()) {
      BOOST_THROW_EXCEPTION(InvalidOperation("Changing master endpoint is not supported"));
    }
  }

  // Adjust size of processors_; cast size to int to avoid compiler warning.
  while (static_cast<int>(processors_.size()) > config.processors_count()) processors_.pop_back();
  while (static_cast<int>(processors_.size()) < config.processors_count()) {
    processors_.push_back(
      std::shared_ptr<Processor>(new Processor(
        &processor_queue_,
        &merger_queue_,
        *merger_,
        schema_)));
  }
}

bool Instance::RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) {
  std::shared_ptr<const ::artm::core::TopicModel> ttm = merger_->GetLatestTopicModel(model_name);
  if (ttm == nullptr) return false;
  ttm->RetrieveExternalTopicModel(topic_model);
  return true;
}

}  // namespace core
}  // namespace artm
