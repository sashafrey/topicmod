#ifndef TOPICMD_CPP_H
#define TOPICMD_CPP_H

#include "topicmd/messages.pb.h"

namespace topicmd {
  int commit_generation(int instance_id, int generation_id);

  int configure_logger(const LoggerConfig& logger_config);

  // Creates an instance and returns the corresponding ID (positive 
  // integer). If argument instance_id is set to 0, then the ID will be
  // picked up automatically. Otherwise, the specified value will be 
  // used if it is available. If an instance with specified ID is 
  // already running the function will return TOPICMD_ERROR. 
  int create_instance(int instance_id, 
		      const InstanceConfig& instance_config);

  int create_model(int instance_id,
		   int model_id,
		   const ModelConfig& model_config);

  void dispose_instance(int instance_id);

  void dispose_model(int instance_id, int model_id);

  void dispose_request(int request_id);

  int finish_generation(int instance_id);

  int insert_batch(int instance_id, const Batch& batch);

  int publish_generation(int instance_id, int generation_id);

  int reconfigure_instance(int instance_id,
			   const InstanceConfig& instance_config);

  int reconfigure_model(int instance_id,
			int model_id,
			const ModelConfig& model_config);

  int request_batch_topics(int instance_id,
			   int model_id,
			   const Batch& batch,
			   BatchTopics* batch_topics);

  int request_model_topics(int instance_id,
			   int model_id,
			   ModelTopics* model_topics);

  int run_tuning_iteration(int instance_id);
} // namespace topicmd

#endif // TOPICMD_CPP_H
