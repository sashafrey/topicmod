// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MERGER_H_
#define SRC_ARTM_MERGER_H_

#include <assert.h>
#include <stdlib.h>

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "artm/common.h"
#include "artm/instance_schema.h"
#include "artm/internals.pb.h"
#include "artm/thread_safe_holder.h"
#include "artm/topic_model.h"
#include "artm/memcached_service.rpcz.h"

#include "rpcz/sync_event.hpp"

namespace artm {
namespace core {

class Merger : boost::noncopyable {
 public:
  Merger(boost::mutex* merger_queue_lock,
         std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue,
         ThreadSafeHolder<InstanceSchema>* schema,
         ThreadSafeHolder<artm::memcached::MemcachedService_Stub>* memcached_service);

  ~Merger();

  void DisposeModel(ModelId model_id);
  void UpdateModel(const ModelConfig& model);
  void ForceSyncWithMemcached(ModelId model_id);

  std::shared_ptr<const ::artm::core::TopicModel> GetLatestTopicModel(ModelId model_id) const;

 private:
  enum MergerTaskType {
    kDisposeModel,
    kForceSyncWithMemcached,
  };

  struct MergerTask {
    MergerTaskType task_type;
    ModelId model_id;
    rpcz::sync_event* sync_event;
  };

  mutable boost::mutex lock_;
  ThreadSafeCollectionHolder<ModelId, TopicModel> topic_model_;
  std::map<ModelId, std::shared_ptr<TopicModel>> new_topic_model_;
  ThreadSafeHolder<InstanceSchema>* schema_;
  ThreadSafeHolder<artm::memcached::MemcachedService_Stub>* memcached_service_;

  boost::mutex* merger_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue_;

  ThreadSafeQueue<MergerTask> internal_task_queue_;

  boost::thread thread_;
  void ThreadFunction();

  void SyncWithMemcached(ModelId model_id);
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_MERGER_H_
