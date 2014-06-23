// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_MERGER_H_
#define SRC_ARTM_CORE_MERGER_H_

#include <assert.h>
#include <stdlib.h>

#include <atomic>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "rpcz/sync_event.hpp"

#include "artm/internals.pb.h"

#include "artm/core/common.h"
#include "artm/core/internals.rpcz.h"
#include "artm/core/thread_safe_holder.h"

namespace artm {
namespace core {

class TopicModel;
class InstanceSchema;

class Merger : boost::noncopyable {
 public:
  Merger(boost::mutex* merger_queue_lock,
         std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue,
         ThreadSafeHolder<InstanceSchema>* schema,
         ThreadSafeHolder<artm::core::MasterComponentService_Stub>* master_component_service);

  ~Merger();

  void DisposeModel(ModelName model_name);
  void UpdateModel(const ModelConfig& model);
  void ForceResetScores(ModelName model_name);
  void ForceSyncWithMemcached(ModelName model_name);
  void InvokePhiRegularizers();

  std::shared_ptr<const ::artm::core::TopicModel> GetLatestTopicModel(ModelName model_name) const;

 private:
  enum MergerTaskType {
    kDisposeModel,
    kForceSyncWithMemcached,
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

  mutable boost::mutex lock_;
  ThreadSafeCollectionHolder<ModelName, TopicModel> topic_model_;
  std::map<ModelName, std::shared_ptr<TopicModel>> new_topic_model_;
  ThreadSafeHolder<InstanceSchema>* schema_;
  ThreadSafeHolder<artm::core::MasterComponentService_Stub>* master_component_service_;

  boost::mutex* merger_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue_;

  ThreadSafeQueue<MergerTask> internal_task_queue_;

  mutable std::atomic<bool> is_stopping;
  boost::thread thread_;
  void ThreadFunction();

  void SyncWithMemcached(ModelName model_name);
  void ResetScores(ModelName model_name);
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_CORE_MERGER_H_
