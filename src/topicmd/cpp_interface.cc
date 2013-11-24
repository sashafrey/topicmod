#include "cpp_interface.h"
#include "messages.pb.h"

#define TOPICMD_ERROR -1
#define TOPICMD_SUCCESS 0

namespace topicmd {
  int commit_generation(int instance_id, int generation_id) {
    return TOPICMD_SUCCESS;
  }

  int configure_logger(const LoggerConfig& logger_config)  {
    return TOPICMD_SUCCESS;
  }

  int create_instance(int instance_id, 
		      const InstanceConfig& instance_config) {
    return TOPICMD_SUCCESS;
  }

  int create_model(int instance_id,
		   int model_id,
		   const ModelConfig& model_config) {
    return TOPICMD_SUCCESS;
  }

  void dispose_instance(int instance_id) { }

  void dispose_model(int instance_id, int model_id) { }

  int finish_generation(int instance_id) {
    return TOPICMD_SUCCESS;
  }

  int insert_batch(int instance_id, const Batch& batch) {
    return TOPICMD_SUCCESS;
  }

  int publish_generation(int instance_id, int generation_id) {
    return TOPICMD_SUCCESS;
  }

  int reconfigure_instance(int instance_id,
			   const InstanceConfig& instance_config) {
    return TOPICMD_SUCCESS;
  }

  int reconfigure_model(int instance_id,
			int model_id,
			const ModelConfig& model_config) {
    return TOPICMD_SUCCESS;
  }

  int request_batch_topics(int instance_id,
			   int model_id,
			   const Batch& batch,
			   BatchTopics* batch_topics) {
    return TOPICMD_SUCCESS;
  }

  int request_model_topics(int instance_id,
			   int model_id,
			   ModelTopics* model_topics) {
    for (int i = 0; i < 123; i++) {
      model_topics->add_token_topic();
    }

    return TOPICMD_SUCCESS;
  }

  int run_tuning_iteration(int instance_id) {
    return TOPICMD_SUCCESS;
  }

} // namespace topicmd
