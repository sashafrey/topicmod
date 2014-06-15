// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_INSTANCE_H_
#define SRC_ARTM_CORE_INSTANCE_H_

#include <map>
#include <memory>
#include <queue>
#include <vector>
#include <string>

#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "rpcz/application.hpp"

#include "artm/messages.pb.h"

#include "artm/core/common.h"
#include "artm/core/dictionary.h"
#include "artm/core/internals.pb.h"
#include "artm/core/internals.rpcz.h"
#include "artm/core/template_manager.h"
#include "artm/core/thread_safe_holder.h"

namespace artm {
namespace core {

class Processor;
class Merger;
class InstanceSchema;

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
  // Returns true if succeeded, and false if model_name hasn't been found.
  bool RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model);

  // Reconfigures topic model if already exists, otherwise creates a new model.
  void ReconfigureModel(const ModelConfig& config);

  void DisposeModel(ModelName model_name);
  void Reconfigure(const InstanceConfig& config);
  void AddBatchIntoProcessorQueue(std::shared_ptr<const ProcessorInput> input);
  void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  void DisposeRegularizer(const std::string& name);
  void CreateOrReconfigureDictionary(const DictionaryConfig& config);
  void DisposeDictionary(const std::string& name);
  void ForceResetScores(ModelName model_name);
  void ForceSyncWithMemcached(ModelName model_name);
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
  std::shared_ptr<Merger> merger_;

  // creates background threads for processing
  std::vector<std::shared_ptr<Processor> > processors_;

  ThreadSafeCollectionHolder<std::string, DictionaryMap> dictionaries_;
};

typedef TemplateManager<Instance, InstanceConfig> InstanceManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_INSTANCE_H_
