// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/c_interface.h"

#include <string>

#include "artm/common.h"
#include "artm/messages.pb.h"
#include "artm/instance.h"
#include "artm/exceptions.h"
#include "artm/data_loader.h"

std::string message;

inline char* StringAsArray(std::string* str) {
  return str->empty() ? NULL : &*str->begin();
}

// ToDo(alfrey) log exception to file.
#define CATCH_EXCEPTIONS                                    \
catch (const artm::core::UnsupportedReconfiguration&) {     \
  return ARTM_UNSUPPORTED_RECONFIGURATION;                  \
} catch (const std::runtime_error&) {                       \
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

// =========================================================================
// Data loader interface
// =========================================================================

int ArtmAddBatch(int data_loader_id, int length, const char* batch_blob) {
  try {
    artm::Batch batch;
    if (!batch.ParseFromArray(batch_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
    if (data_loader == nullptr) return ARTM_OBJECT_NOT_FOUND;
    data_loader->AddBatch(batch);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmCreateDataLoader(int data_loader_id, int length, const char* config_blob) {
  try {
    artm::DataLoaderConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    if (data_loader_id > 0) {
      bool succeeded = artm::core::DataLoaderManager::singleton().TryCreate(data_loader_id, config);
      return succeeded ? ARTM_SUCCESS : ARTM_GENERAL_ERROR;
    } else {
      int retval = artm::core::DataLoaderManager::singleton().Create(config);
      assert(retval > 0);
      return retval;
    }
  } CATCH_EXCEPTIONS;
}

int ArtmInvokeIteration(int data_loader_id, int iterations_count) {
  try {
    auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
    if (data_loader == nullptr) return ARTM_OBJECT_NOT_FOUND;
    data_loader->InvokeIteration(iterations_count);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmReconfigureDataLoader(int data_loader_id, int length, const char* config_blob) {
  try {
    artm::DataLoaderConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
    if (data_loader == nullptr) return ARTM_OBJECT_NOT_FOUND;
    data_loader->Reconfigure(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmWaitIdleDataLoader(int data_loader_id) {
  try {
    auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
    if (data_loader == nullptr) return ARTM_OBJECT_NOT_FOUND;
    data_loader->WaitIdle();
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

void ArtmDisposeDataLoader(int data_loader_id) {
  artm::core::DataLoaderManager::singleton().Erase(data_loader_id);
}

// =========================================================================
// Instance interface
// =========================================================================

int ArtmCreateInstance(int instance_id, int length, const char* config_blob) {
  try {
    artm::InstanceConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    if (instance_id > 0) {
      bool succeeded = artm::core::InstanceManager::singleton().TryCreate(instance_id, config);
      return succeeded ? ARTM_SUCCESS : ARTM_OBJECT_NOT_FOUND;
    } else {
      int retval = artm::core::InstanceManager::singleton().Create(config);
      assert(retval > 0);
      return retval;
    }
  } CATCH_EXCEPTIONS;
}

int ArtmCreateModel(int instance_id, int length, const char* config_blob) {
  try {
    artm::ModelConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
    if (instance == nullptr) return ARTM_OBJECT_NOT_FOUND;
    int retval = instance->CreateModel(config);
    assert(retval > 0);
    return retval;
  } CATCH_EXCEPTIONS;
}

int ArtmReconfigureInstance(int instance_id, int length, const char* config_blob) {
  try {
    artm::InstanceConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
    if (instance == nullptr) return ARTM_OBJECT_NOT_FOUND;
    instance->Reconfigure(config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmReconfigureModel(int instance_id, int model_id, int length, const char* config_blob) {
  try {
    artm::ModelConfig config;
    if (!config.ParseFromArray(config_blob, length)) {
      return ARTM_INVALID_MESSAGE;
    }

    auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
    if (instance == nullptr) return ARTM_OBJECT_NOT_FOUND;
    instance->ReconfigureModel(model_id, config);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmRequestBatchTopics(int instance_id, int model_id, int batch_length,
                           const char* batch_blob, int *length, const char** result) {
  try {
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

int ArtmRequestModelTopics(int instance_id, int model_id, int *length, char **address) {
  try {
    artm::ModelTopics model_topics;
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
    if (instance == nullptr) return ARTM_OBJECT_NOT_FOUND;

    instance->RequestModelTopics(model_id, &model_topics);
    model_topics.SerializeToString(&message);
    *length = message.size();
    *address = StringAsArray(&message);
    return ARTM_SUCCESS;
  } CATCH_EXCEPTIONS;
}

void ArtmDisposeInstance(int instance_id) {
  artm::core::InstanceManager::singleton().Erase(instance_id);
}

void ArtmDisposeModel(int instance_id, int model_id) {
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return;
  instance->DisposeModel(model_id);
}

void ArtmDisposeRequest(int request_id) {}
