#include "c_interface.h"

#include <string>
using namespace std;

#include "messages.pb.h"

string message;
inline char* string_as_array(string* str) {
  return str->empty() ? NULL : &*str->begin();
}

int commit_generation(int instance_id, int generation_id) {
  return TOPICMD_SUCCESS;
}

int configure_logger(int length, const char* logger_config) {
  return TOPICMD_SUCCESS;
}

int copy_request_result(int request_id, int length, char* address) {
  memcpy(address, string_as_array(&message), length);
  return TOPICMD_SUCCESS;
}

int create_instance(int instance_id, 
		    int length, 
		    const char* instance_config_blob) 
{
  return TOPICMD_SUCCESS;
}

int create_model(int instance_id,
		 int model_id,
		 int length, 
		 const char* model_config_blob) 
{
  return TOPICMD_SUCCESS;
}

void dispose_instance(int instance_id) {
}

void dispose_model(int instance_id, int model_id) {
}

void dispose_request(int request_id) {
}

int finish_generation(int instance_id) {
  return TOPICMD_SUCCESS;
}

int insert_batch(int instance_id, int length, const char* batch_blob) {
  return TOPICMD_SUCCESS;
}

int publish_generation(int instance_id, int generation_id) {
  return TOPICMD_SUCCESS;
}

  int reconfigure_instance(int instance_id,
			   int length, 
			   const char* instance_config_blob) {
  return TOPICMD_SUCCESS;
}

int reconfigure_model(int instance_id,
		      int model_id,
		      int length,
		      const char* model_config_blob) 
{
  return TOPICMD_SUCCESS;
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
  for (int i = 0; i < 123; i++) {
    model_topics.add_token_topic();
  }

  model_topics.SerializeToString(&message);
  *length = message.size();
  *address = string_as_array(&message);
  return TOPICMD_SUCCESS;
}

int run_tuning_iteration(int instance_id) 
{
  return TOPICMD_SUCCESS;
}
