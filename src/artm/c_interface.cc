// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/c_interface.h"

#include <string>

#include "boost/thread/tss.hpp"

#include "glog/logging.h"

#include "rpcz/rpc.hpp"

#include "artm/messages.pb.h"
#include "artm/score_calculator_interface.h"
#include "artm/core/common.h"
#include "artm/core/helpers.h"
#include "artm/core/master_component.h"
#include "artm/core/master_proxy.h"
#include "artm/core/node_controller.h"

// Never use the following variables explicitly (only through the corresponding methods).
// It might be good idea to make them a private members of a new singleton class.
static boost::thread_specific_ptr<std::string> last_message_;
static boost::thread_specific_ptr<std::string> last_error_;

static std::string* last_message() {
  if (last_message_.get() == nullptr) {
    last_message_.reset(new std::string());
  }

  return last_message_.get();
}

static void set_last_error(const std::string& error) {
  if (last_error_.get() == nullptr) {
    last_error_.reset(new std::string());
  }

  last_error_->assign(error);
}


static void EnableLogging() {
  static bool logging_enabled = false;
  if (!logging_enabled) {
    logging_enabled = true;
    FLAGS_log_dir = ".";
    FLAGS_logbufsecs = 0;
    ::google::InitGoogleLogging(".");
  }
}

inline char* StringAsArray(std::string* str) {
  return str->empty() ? NULL : &*str->begin();
}

// =========================================================================
// Common routines
// =========================================================================

int ArtmCopyRequestResult(int length, char* address) {
  if (length != last_message()->size()) {
    set_last_error("ArtmCopyRequestResult() called with invalid 'length' parameter.");
    return ARTM_INVALID_OPERATION;
  }

  memcpy(address, StringAsArray(last_message()), length);
  return ARTM_SUCCESS;
}

const char* ArtmGetLastErrorMessage() {
  if (last_error_.get() == nullptr) {
    return nullptr;
  }

  return last_error_->c_str();
}

int ArtmSaveBatch(const char* disk_path, int length, const char* batch) {
  try {
    artm::Batch batch_object;
    if (!batch_object.ParseFromArray(batch, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    artm::Batch compacted_batch;
    artm::core::BatchHelpers::CompactBatch(batch_object, &compacted_batch);
    artm::core::BatchHelpers::SaveBatch(compacted_batch, std::string(disk_path));
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmAddBatch(int master_id, int length, const char* batch) {
  try {
    artm::Batch batch_object;
    if (!batch_object.ParseFromArray(batch, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    master_component->AddBatch(batch_object);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmInvokeIteration(int master_id, int iterations_count) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    master_component->InvokeIteration(iterations_count);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmWaitIdle(int master_id, int timeout_milliseconds) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    bool result = master_component->WaitIdle(timeout_milliseconds);

    if (result) {
      return ARTM_SUCCESS;
    } else {
      return ARTM_STILL_WORKING;
    }
  } CATCH_EXCEPTIONS;
}

// =========================================================================
// MasterComponent / MasterProxy
// =========================================================================

int ArtmCreateMasterProxy(int length, const char* master_proxy_config) {
  try {
    EnableLogging();

    artm::MasterProxyConfig config;
    if (!config.ParseFromArray(master_proxy_config, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto& mcm = artm::core::MasterComponentManager::singleton();
    int retval = mcm.Create<::artm::core::MasterProxy, ::artm::MasterProxyConfig>(config);
    assert(retval > 0);
    return retval;
  } CATCH_EXCEPTIONS;
}

int ArtmCreateMasterComponent(int length, const char* master_component_config) {
  try {
    EnableLogging();

    artm::MasterComponentConfig config;
    if (!config.ParseFromArray(master_component_config, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto& mcm = artm::core::MasterComponentManager::singleton();
    int retval = mcm.Create<::artm::core::MasterComponent, ::artm::MasterComponentConfig>(config);
    assert(retval > 0);
    return retval;
  } CATCH_EXCEPTIONS;
}

int ArtmCreateModel(int master_id, int length, const char* model_config) {
  return ArtmReconfigureModel(master_id, length, model_config);
}

int ArtmReconfigureMasterComponent(int master_id, int length, const char* master_component_config) {
  try {
    artm::MasterComponentConfig config;
    if (!config.ParseFromArray(master_component_config, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    master_component->Reconfigure(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmReconfigureModel(int master_id, int length, const char* model_config) {
  try {
    artm::ModelConfig config;
    if (!config.ParseFromArray(model_config, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    master_component->CreateOrReconfigureModel(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmRequestThetaMatrix(int master_id, const char* model_name) {
  try {
    artm::ThetaMatrix theta_matrix;
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

    master_component->RequestThetaMatrix(model_name, &theta_matrix);
    theta_matrix.SerializeToString(last_message());
    return last_message()->size();
  } CATCH_EXCEPTIONS;
}

int ArtmRequestTopicModel(int master_id, const char* model_name) {
  try {
    artm::TopicModel topic_model;
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

    master_component->RequestTopicModel(model_name, &topic_model);
    topic_model.SerializeToString(last_message());
    return last_message()->size();
  } CATCH_EXCEPTIONS;
}

int ArtmRequestRegularizerState(int master_id, const char* regularizer_name) {
  try {
    artm::RegularizerInternalState regularizer_state;
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

    master_component->RequestRegularizerState(regularizer_name, &regularizer_state);
    regularizer_state.SerializeToString(last_message());
    return last_message()->size();
  } CATCH_EXCEPTIONS;
}

int ArtmRequestScore(int master_id, const char* model_name, const char* score_name) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

    ::artm::ScoreData score_data;
    master_component->RequestScore(model_name, score_name, &score_data);
    score_data.SerializeToString(last_message());
    return last_message()->size();
  } CATCH_EXCEPTIONS;
}

int ArtmOverwriteTopicModel(int master_id, int length, const char* topic_model) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

    artm::TopicModel topic_model_object;
    if (!topic_model_object.ParseFromArray(topic_model, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    master_component->OverwriteTopicModel(topic_model_object);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmDisposeMasterComponent(int master_id) {
  artm::core::MasterComponentManager::singleton().Erase(master_id);
  return ARTM_SUCCESS;
}

int ArtmCreateNodeController(int length, const char* node_controller_config) {
  try {
    EnableLogging();

    artm::NodeControllerConfig config;
    if (!config.ParseFromArray(node_controller_config, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto& ncm = artm::core::NodeControllerManager::singleton();
    int retval = ncm.Create<::artm::core::NodeController, ::artm::NodeControllerConfig>(config);
    assert(retval > 0);
    return retval;
  } CATCH_EXCEPTIONS;
}

int ArtmDisposeNodeController(int node_controller_id) {
  artm::core::NodeControllerManager::singleton().Erase(node_controller_id);
  return ARTM_SUCCESS;
}

int ArtmDisposeModel(int master_id, const char* model_name) {
  auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
  if (master_component == nullptr) return ARTM_SUCCESS;
  master_component->DisposeModel(model_name);
  return ARTM_SUCCESS;
}

int ArtmCreateRegularizer(int master_id, int length, const char* regularizer_config) {
  return ArtmReconfigureRegularizer(master_id, length, regularizer_config);
}

int ArtmReconfigureRegularizer(int master_id, int length, const char* regularizer_config) {
  try {
    artm::RegularizerConfig config;
    if (!config.ParseFromArray(regularizer_config, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    master_component->CreateOrReconfigureRegularizer(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmDisposeRegularizer(int master_id, const char* regularizer_name) {
  auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
  if (master_component == nullptr) return ARTM_SUCCESS;
  master_component->DisposeRegularizer(regularizer_name);
  return ARTM_SUCCESS;
}

int ArtmInvokePhiRegularizers(int master_id) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component != nullptr) {
      master_component->InvokePhiRegularizers();
    }

    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmCreateDictionary(int master_id, int length, const char* dictionary_config) {
  return ArtmReconfigureDictionary(master_id, length, dictionary_config);
}

int ArtmReconfigureDictionary(int master_id, int length, const char* dictionary_config) {
  try {
    artm::DictionaryConfig config;
    if (!config.ParseFromArray(dictionary_config, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    master_component->CreateOrReconfigureDictionary(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmDisposeDictionary(int master_id, const char* dictionary_name) {
  auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
  if (master_component == nullptr) return ARTM_SUCCESS;
  master_component->DisposeDictionary(dictionary_name);
  return ARTM_SUCCESS;
}
