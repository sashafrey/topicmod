// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/c_interface.h"

#include <string>

#include "artm/common.h"
#include "artm/messages.pb.h"
#include "artm/instance.h"
#include "artm/data_loader.h"

std::string message;

inline char* StringAsArray(std::string* str) {
  return str->empty() ? NULL : &*str->begin();
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
  artm::Batch batch;
  batch.ParseFromArray(batch_blob, length);

  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  return data_loader->AddBatch(batch);
}

int ArtmCreateDataLoader(int data_loader_id, int length, const char* config_blob) {
  artm::DataLoaderConfig config;
  config.ParseFromArray(config_blob, length);
  return artm::core::DataLoaderManager::singleton().Create(data_loader_id, config);
}

int ArtmInvokeIteration(int data_loader_id, int iterations_count) {
  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  return data_loader->InvokeIteration(iterations_count);
}

int ArtmReconfigureDataLoader(int data_loader_id, int length, const char* config_blob) {
  artm::DataLoaderConfig config;
  config.ParseFromArray(config_blob, length);

  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  return data_loader->Reconfigure(config);
}

int ArtmWaitIdleDataLoader(int data_loader_id) {
  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  data_loader->WaitIdle();
  return ARTM_SUCCESS;
}

void ArtmDisposeDataLoader(int data_loader_id) {
  artm::core::DataLoaderManager::singleton().Erase(data_loader_id);
}

// =========================================================================
// Instance interface
// =========================================================================

int ArtmCreateInstance(int instance_id, int length, const char* config_blob) {
  artm::InstanceConfig config;
  config.ParseFromArray(config_blob, length);
  return artm::core::InstanceManager::singleton().Create(instance_id, config);
}

int ArtmCreateModel(int instance_id, int length, const char* config_blob) {
  artm::ModelConfig config;
  config.ParseFromArray(config_blob, length);

  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;
  return instance->CreateModel(config);
}

int ArtmReconfigureInstance(int instance_id, int length, const char* config_blob) {
  artm::InstanceConfig config;
  config.ParseFromArray(config_blob, length);
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;
  return instance->Reconfigure(config);
}

int ArtmReconfigureModel(int instance_id, int model_id, int length, const char* config_blob) {
  artm::ModelConfig config;
  config.ParseFromArray(config_blob, length);

  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;
  return instance->ReconfigureModel(model_id, config);
}

int ArtmRequestBatchTopics(int instance_id, int model_id, int batch_length,
                           const char* batch_blob, int *length, const char** result) {
  return ARTM_SUCCESS;
}

int ArtmRequestModelTopics(int instance_id, int model_id, int *length, char **address) {
  artm::ModelTopics model_topics;
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;

  instance->RequestModelTopics(model_id, &model_topics);
  model_topics.SerializeToString(&message);
  *length = message.size();
  *address = StringAsArray(&message);
  return ARTM_SUCCESS;
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
