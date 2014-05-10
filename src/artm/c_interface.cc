// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/c_interface.h"

#include <string>

#include "artm/common.h"
#include "artm/master_component.h"
#include "artm/messages.pb.h"
#include "artm/exceptions.h"
#include "artm/memcached_server.h"

#include "artm/dirichlet_regularizer_theta.h"
#include "artm/dirichlet_regularizer_phi.h"
#include "artm/regularizer_interface.h"

#include "rpcz/rpc.hpp"
#include "glog/logging.h"

std::string message;

inline char* StringAsArray(std::string* str) {
  return str->empty() ? NULL : &*str->begin();
}

#define CATCH_EXCEPTIONS                                    \
catch (const rpcz::rpc_error& e) {                          \
  LOG(ERROR) << "rpc_error: " << e.what();                  \
  return ARTM_NETWORK_ERROR;                                \
} catch (const artm::core::NetworkException& e) {           \
  LOG(ERROR) << "NetworkException: " << e.what();           \
  return ARTM_NETWORK_ERROR;                                \
} catch (const artm::core::UnsupportedReconfiguration& e) { \
  LOG(ERROR) << "UnsupportedReconfiguration: " << e.what(); \
  return ARTM_UNSUPPORTED_RECONFIGURATION;                  \
} catch (const std::runtime_error& e) {                     \
  LOG(ERROR) << "runtime_error: " << e.what();              \
  return ARTM_GENERAL_ERROR;                                \
} catch (...) {                                             \
  LOG(ERROR) << "unknown error.";                           \
  return ARTM_GENERAL_ERROR;                                \
}

// =========================================================================
// Common routines
// =========================================================================

int ArtmConfigureLogger(int length, const char* logger_config) {
  return ARTM_SUCCESS;
}

int ArtmCopyRequestResult(int request_id, int length, char* address) {
  memcpy(address, StringAsArray(&message), length);
  return ARTM_SUCCESS;
}

int ArtmGetRequestLength(int request_id) {
  return message.size();
}

// ===============================================================================================
// Memcached service - host
// ===============================================================================================
DLL_PUBLIC int ArtmCreateMemcachedServer(const char* endpoint) {
  try {
    return artm::core::MemcachedServerManager::singleton().Create(std::string(endpoint));
  } CATCH_EXCEPTIONS;
}

DLL_PUBLIC int ArtmDisposeMemcachedServer(int memcached_server_id) {
  try {
    artm::core::MemcachedServerManager::singleton().Erase(memcached_server_id);
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

int ArtmWaitIdle(int master_id) {
  try {
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;
    master_component->WaitIdle();
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

// =========================================================================
// MasterComponent interface
// =========================================================================

int ArtmCreateMasterComponent(int master_id, int length, const char* config_blob) {
  try {
    artm::MasterComponentConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    if (master_id > 0) {
      bool succeeded = artm::core::MasterComponentManager::singleton().TryCreate(master_id, config);
      return succeeded ? ARTM_SUCCESS : ARTM_OBJECT_NOT_FOUND;
    } else {
      int retval = artm::core::MasterComponentManager::singleton().Create(config);
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
    master_component->ReconfigureModel(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmRequestBatchTopics(int master_id, const char* model_id, int batch_length,
                           const char* batch_blob) {
  try {
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmRequestTopicModel(int master_id, const char* model_id) {
  try {
    artm::TopicModel topic_model;
    auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
    if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

    master_component->RequestTopicModel(model_id, &topic_model);
    topic_model.SerializeToString(&message);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

void ArtmDisposeMasterComponent(int master_id) {
  artm::core::MasterComponentManager::singleton().Erase(master_id);
}

void ArtmDisposeModel(int master_id, const char* model_id) {
  auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
  if (master_component == nullptr) return;
  master_component->DisposeModel(model_id);
}

void ArtmDisposeRequest(int request_id) {}

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
    std::string regularizer_name = config.name();
    artm::RegularizerConfig_Type regularizer_type = config.type();
    std::string config_blob = config.config();

    // add here new case if adding new regularizer
    switch (regularizer_type) {
    case artm::RegularizerConfig_Type_DirichletRegularizerTheta: {
      artm::DirichletRegularizerThetaConfig regularizer_config;
      if (!regularizer_config.ParseFromArray(config_blob.c_str(), config_blob.length())) {
        return ARTM_INVALID_MESSAGE;
      }

      std::shared_ptr<artm::core::RegularizerInterface> regularizer(
        new artm::core::DirichletRegularizerTheta(regularizer_config));
      auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
      if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

      master_component->CreateOrReconfigureRegularizer(regularizer_name, regularizer);
      return ARTM_SUCCESS;
    }
    case artm::RegularizerConfig_Type_DirichletRegularizerPhi: {
      artm::DirichletRegularizerPhiConfig regularizer_config;
      if (!regularizer_config.ParseFromArray(config_blob.c_str(), config_blob.length())) {
        return ARTM_INVALID_MESSAGE;
      }

      std::shared_ptr<artm::core::RegularizerInterface> regularizer(
        new artm::core::DirichletRegularizerPhi(regularizer_config));
      auto master_component = artm::core::MasterComponentManager::singleton().Get(master_id);
      if (master_component == nullptr) return ARTM_OBJECT_NOT_FOUND;

      master_component->CreateOrReconfigureRegularizer(regularizer_name, regularizer);
      return ARTM_SUCCESS;
    }
    default:
      return ARTM_INVALID_MESSAGE;
    }
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
  } CATCH_EXCEPTIONS;
}
