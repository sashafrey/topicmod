// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_MERGER_H_
#define SRC_ARTM_CORE_MERGER_H_

#include <assert.h>
#include <stdlib.h>

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "rpcz/sync_event.hpp"

#include "artm/core/common.h"
#include "artm/core/internals.pb.h"
#include "artm/core/internals.rpcz.h"
#include "artm/core/thread_safe_holder.h"

namespace artm {
namespace core {

class DataLoader;
class TopicModel;
class InstanceSchema;

class Merger : boost::noncopyable {
 public:
  Merger(ThreadSafeQueue<std::shared_ptr<const ProcessorOutput> >* merger_queue,
         ThreadSafeHolder<InstanceSchema>* schema,
         MasterComponentService_Stub* master_component_service,
         DataLoader* data_loader);

  ~Merger();

  void DisposeModel(ModelName model_name);
  void CreateOrReconfigureModel(const ModelConfig& model);
  void ForceResetScores(ModelName model_name);
  void ForcePullTopicModel();
  void ForcePushTopicModelIncrement();
  void InvokePhiRegularizers();
  void OverwriteTopicModel(const ::artm::TopicModel& topic_model);

  std::shared_ptr<const ::artm::core::TopicModel> GetLatestTopicModel(ModelName model_name) const;
  bool RetrieveExternalTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) const;

 private:
  enum MergerTaskType {
    kDisposeModel,
    kForcePullTopicModel,
    kForcePushTopicModelIncrement,
    kForceResetScores,
  };

  struct MergerTask {
    MergerTask() {}
    MergerTask(MergerTaskType _task_type, ModelName _model_name)
        : task_type(_task_type), model_name(_model_name), sync_event(nullptr) {}

    MergerTask(MergerTaskType _task_type, ModelName _model_name, rpcz::sync_event* _sync_event)
        : task_type(_task_type), model_name(_model_name), sync_event(_sync_event) {}

    MergerTaskType task_type;
    ModelName model_name;
    rpcz::sync_event* sync_event;
  };

  ThreadSafeCollectionHolder<ModelName, TopicModel> topic_model_;
  std::map<ModelName, std::shared_ptr<TopicModel>> topic_model_inc_;
  ThreadSafeHolder<InstanceSchema>* schema_;
  artm::core::MasterComponentService_Stub* master_component_service_;

  ThreadSafeQueue<std::shared_ptr<const ProcessorOutput> >* merger_queue_;
  ThreadSafeQueue<MergerTask> internal_task_queue_;

  DataLoader* data_loader_;

  mutable std::atomic<bool> is_stopping;
  boost::thread thread_;
  void ThreadFunction();

  void PullTopicModel();
  void PushTopicModelIncrement();
  void ResetScores(ModelName model_name);
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_CORE_MERGER_H_
