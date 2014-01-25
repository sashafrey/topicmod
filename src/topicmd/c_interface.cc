#include "c_interface.h"

#include <string>
using namespace std;

#include "topicmd/common.h"
#include "topicmd/messages.pb.h"
#include "topicmd/cpp_interface.h"

string message;

inline char* string_as_array(string* str) {
  return str->empty() ? NULL : &*str->begin();
}

int commit_generation(int instance_id, int generation_id) {
  topicmd::commit_generation(instance_id, generation_id);
  return TOPICMD_SUCCESS;
}

int configure_logger(int length, const char* logger_config) {
  return TOPICMD_SUCCESS;
}

int copy_request_result(int request_id, int length, char* address) 
{
  memcpy(address, string_as_array(&message), length);
  return TOPICMD_SUCCESS;
}

int create_instance(
  int instance_id, 
  int length, 
  const char* instance_config_blob) 
{
  topicmd::InstanceConfig instance_config;
  instance_config.ParseFromArray(instance_config_blob, length);
  return topicmd::create_instance(instance_id, instance_config);
}

int create_model(int instance_id,
                 int model_id,
                 int length, 
                 const char* model_config_blob) 
{
  topicmd::ModelConfig model_config;
  model_config.ParseFromArray(model_config_blob, length);
  return topicmd::create_model(instance_id, model_id, model_config);
}

int discard_partition(int instance_id) {
  return topicmd::discard_partition(instance_id);
}

void dispose_instance(int instance_id) {
  topicmd::dispose_instance(instance_id);
}

void dispose_model(int instance_id, int model_id) {
  topicmd::dispose_model(instance_id, model_id);
}

void dispose_request(int request_id) {
}

int finish_partition(int instance_id) {
  return topicmd::finish_partition(instance_id);
}

int insert_batch(int instance_id, int length, const char* batch_blob) {
  topicmd::Batch batch;
  batch.ParseFromArray(batch_blob, length);
  return topicmd::insert_batch(instance_id, batch);  
}

int publish_generation(int instance_id, int generation_id) {
  return topicmd::publish_generation(instance_id, generation_id);
}

int reconfigure_instance(int instance_id,
                         int length, 
                         const char* instance_config_blob) 
{
  topicmd::InstanceConfig instance_config;
  instance_config.ParseFromArray(instance_config_blob, length);
  return topicmd::reconfigure_instance(instance_id, instance_config);
}

int reconfigure_model(int instance_id,
                      int model_id,
                      int length,
                      const char* model_config_blob) 
{
  topicmd::ModelConfig model_config;
  model_config.ParseFromArray(model_config_blob, length);
  return topicmd::reconfigure_model(instance_id, model_id, model_config);
}

int request_batch_topics(int instance_id,
                         int model_id,
                         int batch_length,
                         const char* batch_blob,
                         int *length,
                         const char** result) 
{
  return TOPICMD_SUCCESS;
}

int request_model_topics(int instance_id,
                         int model_id,
                         int *length,
                         char **address) 
{
  topicmd::ModelTopics model_topics;
  topicmd::request_model_topics(instance_id, model_id, &model_topics);
  model_topics.SerializeToString(&message);
  *length = message.size();
  *address = string_as_array(&message);
  return TOPICMD_SUCCESS;
}

int wait_model_processed(int instance_id,
                         int model_id,
                         int processed_items) 
{
  return topicmd::wait_model_processed(instance_id, model_id, processed_items);
}