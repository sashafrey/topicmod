// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_INSTANCE_H_
#define SRC_ARTM_INSTANCE_H_

#include <map>
#include <memory>
#include <queue>
#include <vector>
#include <string>

#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "artm/common.h"
#include "artm/instance_schema.h"
#include "artm/internals.pb.h"
#include "artm/internals.rpcz.h"
#include "artm/merger.h"
#include "artm/messages.pb.h"
#include "artm/processor.h"
#include "artm/template_manager.h"
#include "artm/thread_safe_holder.h"
#include "artm/regularizer_interface.h"

#include "rpcz/application.hpp"


namespace artm {
namespace core {

class Instance : boost::noncopyable {
 public:
  ~Instance();

  int id() const {
    return instance_id_;
  }

  const std::shared_ptr<InstanceSchema> schema() const {
    return schema_.get();
  }

  int processor_queue_size() const;

  // Retrieves topic model.
  // Returns true if succeeded, and false if model_id hasn't been found.
  bool RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model);

  // Reconfigures topic model if already exists, otherwise creates a new model.
  void ReconfigureModel(const ModelConfig& config);

  void DisposeModel(ModelId model_id);
  void Reconfigure(const InstanceConfig& config);
  void AddBatchIntoProcessorQueue(std::shared_ptr<const ProcessorInput> input);

  void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  void DisposeRegularizer(const std::string& name);
  void ForceSyncWithMemcached(ModelId model_id);
  void InvokePhiRegularizers();

 private:
  friend class TemplateManager<Instance, InstanceConfig>;

  // All instances must be created via TemplateManager.
  Instance(int id, const InstanceConfig& config);

  mutable boost::mutex lock_;
  int instance_id_;
  ThreadSafeHolder<InstanceSchema> schema_;

  std::unique_ptr<rpcz::application> application_;
  ThreadSafeHolder<artm::core::MasterComponentService_Stub> master_component_service_proxy_;

  mutable boost::mutex processor_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorInput> > processor_queue_;

  mutable boost::mutex merger_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorOutput> > merger_queue_;

  // creates a background thread that keep merging processor output
  Merger merger_;

  // creates background threads for processing
  std::vector<std::shared_ptr<Processor> > processors_;
};

typedef TemplateManager<Instance, InstanceConfig> InstanceManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_INSTANCE_H_
