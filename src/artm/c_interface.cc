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

int ArtmSaveBatch(const char* disk_path, int length, const char* batch_blob) {
  try {
    artm::Batch batch;
    if (!batch.ParseFromArray(batch_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    artm::Batch compacted_batch;
    artm::core::BatchHelpers::CompactBatch(batch, &compacted_batch);
    artm::core::BatchHelpers::SaveBatch(compacted_batch, std::string(disk_path));
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmAddBatch(int master_id, int length, const char* batch_blob) {
  try {
    artm::Batch batch;
    if (!batch.ParseFromArray(batch_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    master_component->AddBatch(batch);
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

int ArtmWaitIdle(int master_id, int timeout) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    bool result = master_component->WaitIdle(timeout);

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

int ArtmCreateMasterProxy(int master_id, int length, const char* config_blob) {
  try {
    EnableLogging();

    artm::MasterProxyConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto& mcm = artm::core::MasterComponentManager::singleton();
    if (master_id > 0) {
      bool succeeded = mcm.TryCreate<::artm::core::MasterProxy,
                                     ::artm::MasterProxyConfig>(master_id, config);

      return succeeded ? ARTM_SUCCESS : ARTM_OBJECT_NOT_FOUND;
    } else {
      int retval = mcm.Create<::artm::core::MasterProxy, ::artm::MasterProxyConfig>(config);
      assert(retval > 0);
      return retval;
    }
  } CATCH_EXCEPTIONS;
}

int ArtmCreateMasterComponent(int master_id, int length, const char* config_blob) {
  try {
    EnableLogging();

    artm::MasterComponentConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto& mcm = artm::core::MasterComponentManager::singleton();
    if (master_id > 0) {
      bool ok = mcm.TryCreate<::artm::core::MasterComponent,
                              ::artm::MasterComponentConfig>(master_id, config);

      return ok ? ARTM_SUCCESS : ARTM_OBJECT_NOT_FOUND;
    } else {
      int retval = mcm.Create<::artm::core::MasterComponent,
                              ::artm::MasterComponentConfig>(config);

      assert(retval > 0);
      return retval;
    }
  } CATCH_EXCEPTIONS;
}

int ArtmCreateModel(int master_id, int length, const char* config_blob) {
  return ArtmReconfigureModel(master_id, length, config_blob);
}

int ArtmReconfigureMasterComponent(int master_id, int length, const char* config_blob) {
  try {
    artm::MasterComponentConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    master_component->Reconfigure(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmReconfigureModel(int master_id, int length, const char* config_blob) {
  try {
    artm::ModelConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
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

int ArtmOverwriteTopicModel(int master_id, int length, const char* topic_model_blob) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

    artm::TopicModel topic_model;
    if (!topic_model.ParseFromArray(topic_model_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    master_component->OverwriteTopicModel(topic_model);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

void ArtmDisposeMasterComponent(int master_id) {
  artm::core::MasterComponentManager::singleton().Erase(master_id);
}

int ArtmCreateNodeController(int node_controller_id, int length, const char* config_blob) {
  try {
    EnableLogging();

    artm::NodeControllerConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto& ncm = artm::core::NodeControllerManager::singleton();
    if (node_controller_id > 0) {
      bool succeeded = ncm.TryCreate<::artm::core::NodeController,
                                     ::artm::NodeControllerConfig>(node_controller_id, config);
      return succeeded ? ARTM_SUCCESS : ARTM_OBJECT_NOT_FOUND;
    } else {
      int retval = ncm.Create<::artm::core::NodeController,
                              ::artm::NodeControllerConfig>(config);
      assert(retval > 0);
      return retval;
    }
  } CATCH_EXCEPTIONS;
}

void ArtmDisposeNodeController(int node_controller_id) {
  artm::core::NodeControllerManager::singleton().Erase(node_controller_id);
}

void ArtmDisposeModel(int master_id, const char* model_name) {
  auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
  if (master_component == nullptr) return;
  master_component->DisposeModel(model_name);
}

int ArtmCreateRegularizer(int master_id, int length,
                          const char* regularizer_config_blob) {
  return ArtmReconfigureRegularizer(master_id, length, regularizer_config_blob);
}

int ArtmReconfigureRegularizer(int master_id, int length,
                               const char* regularizer_config_blob) {
  try {
    artm::RegularizerConfig config;
    if (!config.ParseFromArray(regularizer_config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    master_component->CreateOrReconfigureRegularizer(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

void ArtmDisposeRegularizer(int master_id, const char* regularizer_name) {
  auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
  if (master_component == nullptr) return;
  master_component->DisposeRegularizer(regularizer_name);
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

int ArtmCreateDictionary(int master_id, int length,
                          const char* dictionary_config_blob) {
  return ArtmReconfigureDictionary(master_id, length, dictionary_config_blob);
}

int ArtmReconfigureDictionary(int master_id, int length,
                               const char* dictionary_config_blob) {
  try {
    artm::DictionaryConfig config;
    if (!config.ParseFromArray(dictionary_config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    master_component->CreateOrReconfigureDictionary(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

void ArtmDisposeDictionary(int master_id, const char* dictionary_name) {
  auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
  if (master_component == nullptr) return;
  master_component->DisposeDictionary(dictionary_name);
}
