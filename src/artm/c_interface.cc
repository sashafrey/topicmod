#include "c_interface.h"

#include <string>

#include "artm/common.h"
#include "artm/messages.pb.h"
#include "artm/instance.h"
#include "artm/data_loader.h"

using namespace std;
using namespace ::artm;
using namespace ::artm::core;

string message;

inline char* string_as_array(string* str) {
  return str->empty() ? NULL : &*str->begin();
}

int configure_logger(int length, const char* logger_config) {
  return ARTM_SUCCESS;
}

int copy_request_result(int request_id, int length, char* address) 
{
  memcpy(address, string_as_array(&message), length);
  return ARTM_SUCCESS;
}

int create_data_loader(
  int data_loader_id, 
  int length, 
  const char* data_loader_config_blob) 
{
  DataLoaderConfig config;
  config.ParseFromArray(data_loader_config_blob, length);
  return DataLoaderManager::singleton().Create(data_loader_id, config);
}

int reconfigure_data_loader(int data_loader_id, 
                            int length, 
                            const char* data_loader_config_blob) 
{
  DataLoaderConfig config;
  config.ParseFromArray(data_loader_config_blob, length);
  if (!DataLoaderManager::singleton().Contains(data_loader_id)) {
    return ARTM_ERROR;
  }

  auto data_loader = DataLoaderManager::singleton().Get(data_loader_id);
  return data_loader->Reconfigure(config);
}

int invoke_iteration(int data_loader_id, int iterations_count) {
  if (!DataLoaderManager::singleton().Contains(data_loader_id)) {
    return ARTM_ERROR;
  }

  auto data_loader = DataLoaderManager::singleton().Get(data_loader_id);
  return data_loader->InvokeIteration(iterations_count);
}

int wait_idle_data_loader(int data_loader_id) {
  if (!DataLoaderManager::singleton().Contains(data_loader_id)) {
    return ARTM_ERROR;
  }

  auto data_loader = DataLoaderManager::singleton().Get(data_loader_id);
  return data_loader->WaitIdle();
}

int create_instance(
  int instance_id, 
  int length, 
  const char* instance_config_blob) 
{
  InstanceConfig instance_config;
  instance_config.ParseFromArray(instance_config_blob, length);
  return InstanceManager::singleton().Create(instance_id, instance_config);
}

int create_model(int instance_id,
                 int length, 
                 const char* model_config_blob) 
{
  ModelConfig model_config;
  model_config.ParseFromArray(model_config_blob, length);
  
  if (!InstanceManager::singleton().Contains(instance_id)) {
    return ARTM_ERROR;
  }

  auto instance = InstanceManager::singleton().Get(instance_id);
  return instance->CreateModel(model_config);
}

void dispose_data_loader(int data_loader_id) {
  if (DataLoaderManager::singleton().Contains(data_loader_id)) {
    DataLoaderManager::singleton().Erase(data_loader_id);
  }
}

void dispose_instance(int instance_id) {
  if (InstanceManager::singleton().Contains(instance_id)) {
    InstanceManager::singleton().Erase(instance_id);
  }
}

void dispose_model(int instance_id, int model_id) {
  if (!InstanceManager::singleton().Contains(instance_id)) {
    return;
  }

  auto instance = InstanceManager::singleton().Get(instance_id);
  instance->DisposeModel(model_id);
}

void dispose_request(int request_id) {
}

int add_batch(int data_loader_id, int length, const char* batch_blob) {
  Batch batch;
  batch.ParseFromArray(batch_blob, length);

  if (!DataLoaderManager::singleton().Contains(data_loader_id)) {
    return ARTM_ERROR;
  }

  auto data_loader = DataLoaderManager::singleton().Get(data_loader_id);
  return data_loader->AddBatch(batch);
}

int reconfigure_instance(int instance_id,
                         int length, 
                         const char* instance_config_blob) 
{
  InstanceConfig instance_config;
  instance_config.ParseFromArray(instance_config_blob, length);
  if (!InstanceManager::singleton().Contains(instance_id)) {
    return ARTM_ERROR;
  }

  auto instance = InstanceManager::singleton().Get(instance_id);
  return instance->Reconfigure(instance_config);
}

int reconfigure_model(int instance_id,
                      int model_id,
                      int length,
                      const char* model_config_blob) 
{
  ModelConfig model_config;
  model_config.ParseFromArray(model_config_blob, length);
  
  if (!InstanceManager::singleton().Contains(instance_id)) {
    return ARTM_ERROR;
  }

  auto instance = InstanceManager::singleton().Get(instance_id);
  return instance->ReconfigureModel(model_id, model_config);
}

int request_batch_topics(int instance_id,
                         int model_id,
                         int batch_length,
                         const char* batch_blob,
                         int *length,
                         const char** result) 
{
  return ARTM_SUCCESS;
}

int request_model_topics(int instance_id,
                         int model_id,
                         int *length,
                         char **address) 
{
  ModelTopics model_topics;
  if (!InstanceManager::singleton().Contains(instance_id)) {
    return ARTM_ERROR;
  }

  auto instance = InstanceManager::singleton().Get(instance_id);
  instance->RequestModelTopics(model_id, &model_topics);
  model_topics.SerializeToString(&message);
  *length = message.size();
  *address = string_as_array(&message);
  return ARTM_SUCCESS;
}

int wait_model_processed(int instance_id,
                         int model_id,
                         int processed_items) 
{
  if (!InstanceManager::singleton().Contains(instance_id)) {
    return ARTM_ERROR;
  }

  auto instance = InstanceManager::singleton().Get(instance_id);
  return instance->WaitModelProcessed(model_id, processed_items);
}
