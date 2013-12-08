#include "topicmd/cpp_interface.h"

#include "topicmd/common.h"
#include "topicmd/instance_manager.h"
#include "topicmd/messages.pb.h"

namespace topicmd {
  int commit_generation(int instance_id, int generation_id) {
    return TOPICMD_SUCCESS;
  }

  int configure_logger(const LoggerConfig& logger_config)  {
    return TOPICMD_SUCCESS;
  }

  int create_instance(int instance_id, 
		      const InstanceConfig& instance_config) {
    return InstanceManager::singleton().CreateInstance(instance_id,
						 instance_config);
  }

  int create_model(int instance_id,
		   int model_id,
		   const ModelConfig& model_config) {
    return TOPICMD_SUCCESS;
  }

  int discard_partition(int instance_id) {
    if (!InstanceManager::singleton().has_instance(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().instance(instance_id);
    return instance->DiscardPartition();
  }

  void dispose_instance(int instance_id) { 
    if (InstanceManager::singleton().has_instance(instance_id)) {
      InstanceManager::singleton().erase_instance(instance_id);
    }
  }

  void dispose_model(int instance_id, int model_id) { }

  int finish_partition(int instance_id) {
    if (!InstanceManager::singleton().has_instance(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().instance(instance_id);
    return instance->FinishPartition();
  }

  int insert_batch(int instance_id, const Batch& batch) {
    if (!InstanceManager::singleton().has_instance(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().instance(instance_id);
    return instance->InsertBatch(batch);
  }

  int publish_generation(int instance_id, int generation_id) {
    if (!InstanceManager::singleton().has_instance(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().instance(instance_id);
    return instance->PublishGeneration(generation_id);
  }

  int reconfigure_instance(int instance_id,
			   const InstanceConfig& instance_config) {
    
    if (!InstanceManager::singleton().has_instance(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().instance(instance_id);
    return instance->Reconfigure(instance_config);
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
    if (!InstanceManager::singleton().has_instance(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().instance(instance_id);
    return instance->RunTuningIteration();
  }

} // namespace topicmd

