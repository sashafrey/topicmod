// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_INSTANCE_H_
#define SRC_ARTM_CORE_INSTANCE_H_

#include <map>
#include <memory>
#include <vector>
#include <string>

#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "rpcz/application.hpp"

#include "artm/messages.pb.h"

#include "artm/core/common.h"
#include "artm/core/internals.pb.h"
#include "artm/core/internals.rpcz.h"
#include "artm/core/template_manager.h"
#include "artm/core/thread_safe_holder.h"

namespace artm {
namespace core {

class LocalDataLoader;
class RemoteDataLoader;
class DataLoader;
class Processor;
class Merger;
class InstanceSchema;
typedef std::map<std::string, ::artm::DictionaryEntry> DictionaryMap;
typedef ThreadSafeCollectionHolder<std::string, DictionaryMap> ThreadSafeDictionaryCollection;
typedef ThreadSafeQueue<std::shared_ptr<const ProcessorInput>> ProcessorQueue;
typedef ThreadSafeQueue<std::shared_ptr<const ProcessorOutput>> MergerQueue;

class Instance : boost::noncopyable {
 public:
  ~Instance();

  int id() const { return instance_id_; }
  std::shared_ptr<InstanceSchema> schema() const { return schema_.get(); }
  LocalDataLoader* local_data_loader() { return local_data_loader_.get(); }
  RemoteDataLoader* remote_data_loader() { return remote_data_loader_.get(); }
  DataLoader* data_loader() { return data_loader_; }
  MasterComponentService_Stub* master_component_service_proxy() { return master_component_service_proxy_.get(); }
  ProcessorQueue* processor_queue() { return &processor_queue_; }
  MergerQueue* merger_queue() { return &merger_queue_; }

  // Retrieves topic model.
  // Returns true if succeeded, and false if model_name hasn't been found.
  bool RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model);

  // Reconfigures topic model if already exists, otherwise creates a new model.
  void CreateOrReconfigureModel(const ModelConfig& config);

  void DisposeModel(ModelName model_name);
  void Reconfigure(const MasterComponentConfig& config);
  void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  void DisposeRegularizer(const std::string& name);
  void CreateOrReconfigureDictionary(const DictionaryConfig& config);
  void DisposeDictionary(const std::string& name);
  void ForceResetScores(ModelName model_name);
  void ForcePullTopicModel();
  void ForcePushTopicModelIncrement();
  void InvokePhiRegularizers();
  void OverwriteTopicModel(const ::artm::TopicModel& topic_model);

 private:
  friend class TemplateManager<Instance, MasterComponentConfig>;

  // All instances must be created via TemplateManager.
  Instance(int id, const MasterComponentConfig& config);

  bool is_configured_;

  int instance_id_;
  ThreadSafeHolder<InstanceSchema> schema_;

  std::unique_ptr<rpcz::application> application_;
  std::shared_ptr<artm::core::MasterComponentService_Stub> master_component_service_proxy_;

  ProcessorQueue processor_queue_;
  MergerQueue merger_queue_;

  // creates a background thread that keep merging processor output
  std::shared_ptr<Merger> merger_;

  // creates background threads for processing
  std::vector<std::shared_ptr<Processor> > processors_;

  std::shared_ptr<LocalDataLoader> local_data_loader_;
  std::shared_ptr<RemoteDataLoader> remote_data_loader_;
  DataLoader* data_loader_;

  ThreadSafeDictionaryCollection dictionaries_;
};

typedef TemplateManager<Instance, MasterComponentConfig> InstanceManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_INSTANCE_H_
