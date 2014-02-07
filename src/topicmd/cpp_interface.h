#ifndef TOPICMD_CPP_H
#define TOPICMD_CPP_H

#include "topicmd/messages.pb.h"

namespace topicmd {
  // =========================================================================
  // Common routines
  // =========================================================================

  int configure_logger(const LoggerConfig& logger_config);

  // =========================================================================
  // Data loader interface
  // =========================================================================

  int create_data_loader(int data_loader_id,
                         const DataLoaderConfig& config);

  void dispose_data_loader(int data_loader_id);

  // Adds batch of documents
  int add_batch(int data_loader_id, const Batch& batch);
  
  // =========================================================================
  // Instance interface
  // =========================================================================

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

  int wait_model_processed(int instance_id,
                           int model_id,
                           int processed_items);
} // namespace topicmd

#endif // TOPICMD_CPP_H
