#include "topicmd/cpp_interface.h"

#include "topicmd/common.h"
#include "topicmd/messages.pb.h"
#include "topicmd/instance.h"
#include "topicmd/data_loader.h"

namespace topicmd {
  // =========================================================================
  // Common routines
  // =========================================================================

  int configure_logger(const LoggerConfig& logger_config)  {
    return TOPICMD_SUCCESS;
  }

  // =========================================================================
  // Data loader interface
  // =========================================================================

  int create_data_loader(int data_loader_id,
                         const DataLoaderConfig& config) 
  {
    return DataLoaderManager::singleton().Create(data_loader_id, config);
  }

  void dispose_data_loader(int data_loader_id) 
  {
    if (DataLoaderManager::singleton().Contains(data_loader_id)) {
      DataLoaderManager::singleton().Erase(data_loader_id);
    }
  }

  // Adds batch of documents
  int add_batch(int data_loader_id, const Batch& batch) 
  {
    if (!DataLoaderManager::singleton().Contains(data_loader_id)) {
      return TOPICMD_ERROR;
    }

    auto data_loader = DataLoaderManager::singleton().Get(data_loader_id);
    return data_loader->AddBatch(batch);
  }
  
  // =========================================================================
  // Instance interface
  // =========================================================================

  int create_instance(int instance_id,
                      const InstanceConfig& instance_config) 
  {
    return InstanceManager::singleton().Create(instance_id,
      instance_config);
  }

  int create_model(int instance_id,
                   int model_id,
                   const ModelConfig& model_config) 
  {
    // the same operation.
    return reconfigure_model(instance_id, model_id, model_config);
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

  int reconfigure_instance(int instance_id,
                           const InstanceConfig& instance_config) 
  {
    if (!InstanceManager::singleton().Contains(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().Get(instance_id);
    return instance->Reconfigure(instance_config);
  }

  int reconfigure_model(int instance_id,
                        int model_id,
                        const ModelConfig& model_config) 
  {
    if (!InstanceManager::singleton().Contains(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().Get(instance_id);
    instance->UpdateModel(model_id, model_config);
    return TOPICMD_SUCCESS;
  }

  int request_batch_topics(int instance_id,
                           int model_id,
                           const Batch& batch,
                           BatchTopics* batch_topics) 
  {
    return TOPICMD_SUCCESS;
  }

  int request_model_topics(int instance_id,
                           int model_id,
                           ModelTopics* model_topics) 
  {
    if (!InstanceManager::singleton().Contains(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().Get(instance_id);
    return instance->RequestModelTopics(model_id, model_topics);
  }

  int wait_model_processed(int instance_id,
                           int model_id,
                           int processed_items) 
  {
    if (!InstanceManager::singleton().Contains(instance_id)) {
      return TOPICMD_ERROR;
    }

    auto instance = InstanceManager::singleton().Get(instance_id);
    return instance->WaitModelProcessed(model_id, processed_items);
  }
} // namespace topicmd

