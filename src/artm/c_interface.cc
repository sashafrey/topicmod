#include "c_interface.h"

#include <string>

#include "artm/common.h"
#include "artm/messages.pb.h"
#include "artm/instance.h"
#include "artm/data_loader.h"

std::string message;

inline char* string_as_array(std::string* str) {
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
  artm::DataLoaderConfig config;
  config.ParseFromArray(data_loader_config_blob, length);
  return artm::core::DataLoaderManager::singleton().Create(data_loader_id, config);
}

int reconfigure_data_loader(int data_loader_id,
                            int length,
                            const char* data_loader_config_blob)
{
  artm::DataLoaderConfig config;
  config.ParseFromArray(data_loader_config_blob, length);

  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  return data_loader->Reconfigure(config);
}

int invoke_iteration(int data_loader_id, int iterations_count) {
  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  return data_loader->InvokeIteration(iterations_count);
}

int wait_idle_data_loader(int data_loader_id) {
  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  return data_loader->WaitIdle();
}

int create_instance(
  int instance_id,
  int length,
  const char* instance_config_blob)
{
  artm::InstanceConfig instance_config;
  instance_config.ParseFromArray(instance_config_blob, length);
  return artm::core::InstanceManager::singleton().Create(instance_id, instance_config);
}

int create_model(int instance_id,
                 int length,
                 const char* model_config_blob)
{
  artm::ModelConfig model_config;
  model_config.ParseFromArray(model_config_blob, length);

  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;
  return instance->CreateModel(model_config);
}

void dispose_data_loader(int data_loader_id) {
  artm::core::DataLoaderManager::singleton().Erase(data_loader_id);
}

void dispose_instance(int instance_id) {
  artm::core::InstanceManager::singleton().Erase(instance_id);
}

void dispose_model(int instance_id, int model_id) {
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return;
  instance->DisposeModel(model_id);
}

void dispose_request(int request_id) {
}

int add_batch(int data_loader_id, int length, const char* batch_blob) {
  artm::Batch batch;
  batch.ParseFromArray(batch_blob, length);

  auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  if (data_loader == nullptr) return ARTM_ERROR;
  return data_loader->AddBatch(batch);
}

int reconfigure_instance(int instance_id,
                         int length,
                         const char* instance_config_blob)
{
  artm::InstanceConfig instance_config;
  instance_config.ParseFromArray(instance_config_blob, length);
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;
  return instance->Reconfigure(instance_config);
}

int reconfigure_model(int instance_id,
                      int model_id,
                      int length,
                      const char* model_config_blob)
{
  artm::ModelConfig model_config;
  model_config.ParseFromArray(model_config_blob, length);
 
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;
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
  artm::ModelTopics model_topics;
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id);
  if (instance == nullptr) return ARTM_ERROR;

  instance->RequestModelTopics(model_id, &model_topics);
  model_topics.SerializeToString(&message);
  *length = message.size();
  *address = string_as_array(&message);
  return ARTM_SUCCESS;
}
