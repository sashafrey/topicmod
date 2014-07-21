// Copyright 2014, Additive Regularization of Topic Models.

#include <string>
#include <utility>

#include "artm/core/instance.h"

#include "boost/bind.hpp"

#include "artm/core/common.h"
#include "artm/core/data_loader.h"
#include "artm/core/batch_manager.h"
#include "artm/core/dictionary.h"
#include "artm/core/exceptions.h"
#include "artm/core/processor.h"
#include "artm/core/merger.h"
#include "artm/core/template_manager.h"
#include "artm/core/topic_model.h"
#include "artm/core/zmq_context.h"
#include "artm/core/instance_schema.h"

#include "artm/regularizer_interface.h"
#include "artm/regularizer_sandbox/decorrelator_phi.h"
#include "artm/regularizer_sandbox/dirichlet_theta.h"
#include "artm/regularizer_sandbox/dirichlet_phi.h"
#include "artm/regularizer_sandbox/smooth_sparse_theta.h"
#include "artm/regularizer_sandbox/smooth_sparse_phi.h"

#include "artm/score_calculator_interface.h"
#include "artm/score_sandbox/perplexity.h"
#include "artm/score_sandbox/sparsity_theta.h"
#include "artm/score_sandbox/sparsity_phi.h"

#define CREATE_OR_RECONFIGURE_REGULARIZER(ConfigType, RegularizerType) {                  \
  ConfigType regularizer_config;                                                          \
  if (!regularizer_config.ParseFromArray(config_blob.c_str(), config_blob.length())) {    \
    BOOST_THROW_EXCEPTION(SerializationException("Unable to parse regularizer config"));  \
  }                                                                                       \
  regularizer.reset(new RegularizerType(regularizer_config));                             \
}                                                                                         \

#define CREATE_SCORE_CALCULATOR(ConfigType, ScoreType) {                                  \
  ConfigType score_config;                                                                \
  if (!score_config.ParseFromArray(config_blob.c_str(), config_blob.length())) {          \
    BOOST_THROW_EXCEPTION(SerializationException("Unable to parse score config"));        \
  }                                                                                       \
  score_calculator.reset(new ScoreType(score_config));                                    \
}                                                                                         \

namespace artm {
namespace core {

Instance::Instance(const MasterComponentConfig& config, InstanceType instance_type)
    : is_configured_(false),
      instance_type_(instance_type),
      schema_(std::make_shared<InstanceSchema>(config)),
      dictionaries_(),
      application_(nullptr),
      master_component_service_proxy_(nullptr),
      processor_queue_(),
      merger_queue_(),
      batch_manager_(),
      local_data_loader_(nullptr),
      remote_data_loader_(nullptr),
      merger_(),
      processors_() {
  Reconfigure(config);
}

Instance::~Instance() {}

LocalDataLoader* Instance::local_data_loader() {
  if (!has_local_data_loader()) {
    LOG(ERROR) << "Illegal access to local_data_loader()";
  }

  return local_data_loader_.get();
}

RemoteDataLoader* Instance::remote_data_loader() {
  if (!has_remote_data_loader()) {
    LOG(ERROR) << "Illegal access to remote_data_loader()";
  }

  return remote_data_loader_.get();
}

BatchManager* Instance::batch_manager() {
  if (!has_batch_manager()) {
    LOG(ERROR) << "Illegal access to batch_manager()";
  }

  return batch_manager_.get();
}

MasterComponentService_Stub* Instance::master_component_service_proxy() {
  if (!has_master_component_service_proxy()) {
    LOG(ERROR) << "Illegal access to master_component_service_proxy()";
  }

  return master_component_service_proxy_.get();
}

Merger* Instance::merger() {
  if (!has_merger()) {
    LOG(ERROR) << "Illegal access to merger()()";
  }

  return merger_.get();
}


void Instance::CreateOrReconfigureModel(const ModelConfig& config) {
  if (merger_ != nullptr) {
    merger_->CreateOrReconfigureModel(config);
  }

  auto new_schema = schema_.get_copy();
  new_schema->set_model_config(config.name(), std::make_shared<const ModelConfig>(config));
  schema_.set(new_schema);
}

void Instance::DisposeModel(ModelName model_name) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_model_config(model_name);
  schema_.set(new_schema);

  if (merger_ != nullptr) {
    merger_->DisposeModel(model_name);
  }

  if (batch_manager_ != nullptr) {
    batch_manager_->DisposeModel(model_name);
  }
}

void Instance::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  std::string regularizer_name = config.name();
  artm::RegularizerConfig_Type regularizer_type = config.type();

  std::string config_blob;  // Used by CREATE_OR_RECONFIGURE_REGULARIZER marco
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

    case artm::RegularizerConfig_Type_DecorrelatorPhi: {
      CREATE_OR_RECONFIGURE_REGULARIZER(::artm::DecorrelatorPhiConfig,
                                        ::artm::regularizer_sandbox::DecorrelatorPhi);
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

static std::shared_ptr<ScoreCalculatorInterface> CreateScoreCalculator(const ScoreConfig& config) {
  std::string score_name = config.name();
  artm::ScoreConfig_Type score_type = config.type();

  std::string config_blob;  // Used by CREATE_SCORE_CALCULATOR macro
  if (config.has_config()) {
    config_blob = config.config();
  }

  std::shared_ptr<artm::ScoreCalculatorInterface> score_calculator;

  // add here new case if adding new score
  switch (score_type) {
    case artm::ScoreConfig_Type_Perplexity: {
      CREATE_SCORE_CALCULATOR(::artm::PerplexityScoreConfig,
                              ::artm::score_sandbox::Perplexity);
      break;
    }

    case artm::ScoreConfig_Type_SparsityTheta: {
      CREATE_SCORE_CALCULATOR(::artm::SparsityThetaScoreConfig,
                              ::artm::score_sandbox::SparsityTheta);
      break;
    }

    case artm::ScoreConfig_Type_SparsityPhi: {
      CREATE_SCORE_CALCULATOR(::artm::SparsityPhiScoreConfig,
                              ::artm::score_sandbox::SparsityPhi);
      break;
    }

    default:
      BOOST_THROW_EXCEPTION(SerializationException("Unable to parse score config"));
  }

  return score_calculator;
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

void Instance::Reconfigure(const MasterComponentConfig& master_config) {
  MasterComponentConfig old_config = schema_.get()->config();

  auto new_schema = schema_.get_copy();
  new_schema->set_config(master_config);

  new_schema->clear_score_calculators();  // Clear all score calculators
  for (int score_index = 0;
       score_index < master_config.score_config_size();
       ++score_index) {
    const ScoreConfig& score_config = master_config.score_config(score_index);
    auto score_calculator = CreateScoreCalculator(score_config);
    new_schema->set_score_calculator(score_config.name(), score_calculator);
  }

  schema_.set(new_schema);

  if (!is_configured_) {
    // First reconfiguration.

    // Recreate master_component_service_proxy_;
    if (instance_type_ == NodeControllerInstance) {
      rpcz::application::options options(3);
      options.zeromq_context = ZmqContext::singleton().get();
      application_.reset(new rpcz::application(options));

      master_component_service_proxy_.reset(
        new artm::core::MasterComponentService_Stub(
          application_->create_rpc_channel(master_config.connect_endpoint()), true));
    }

    if (instance_type_ != NodeControllerInstance) {
      batch_manager_.reset(new BatchManager(&schema_));
    }

    // Reconfigure local/remote data loader
    if (instance_type_ == NodeControllerInstance) {
      remote_data_loader_.reset(new RemoteDataLoader(this));
    } else if (instance_type_ == MasterInstanceLocal) {
      local_data_loader_.reset(new LocalDataLoader(this));
    }

    Notifiable* notifiable;
    switch (instance_type_) {
      case MasterInstanceLocal:
        notifiable = local_data_loader_.get();
        break;

      case MasterInstanceNetwork:
        notifiable = nullptr;
        break;

      case NodeControllerInstance:
        notifiable = remote_data_loader_.get();
        break;

      default:
        BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("instance_type_"));
    }

    merger_.reset(new Merger(&merger_queue_, &schema_,
                              master_component_service_proxy_.get(), notifiable));

    is_configured_  = true;
  } else {
    // Second and subsequent reconfiguration - some restrictions apply
    if (old_config.connect_endpoint() !=
        master_config.connect_endpoint()) {
      BOOST_THROW_EXCEPTION(InvalidOperation("Changing master endpoint is not supported"));
    }
  }

  if (instance_type_ != MasterInstanceNetwork) {
    // Adjust size of processors_; cast size to int to avoid compiler warning.
    while (static_cast<int>(processors_.size()) > master_config.processors_count()) {
      processors_.pop_back();
    }

    while (static_cast<int>(processors_.size()) < master_config.processors_count()) {
      processors_.push_back(
        std::shared_ptr<Processor>(new Processor(
          &processor_queue_,
          &merger_queue_,
          *merger_,
          schema_)));
    }
  }
}

}  // namespace core
}  // namespace artm
