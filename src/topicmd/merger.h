#ifndef TOPICMD_MERGER_
#define TOPICMD_MERGER_

#include "assert.h"
#include <stdlib.h>

#include <map>
#include <memory>
#include <string>
#include <queue>

#include <boost/thread.hpp>   
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/generation.h"
#include "topicmd/instance_schema.h"
#include "topicmd/thread_safe_holder.h"
#include "topicmd/token_topic_matrix.h"

namespace topicmd {

class ProcessorOutput : boost::noncopyable {
 public:
  ProcessorOutput(int model_id, int tokens_count, int topics_count) :
      model_id_(model_id),
      tokens_count_(tokens_count),
      topics_count_(topics_count),
      counter_token_topic_(NULL),
      counter_topic_(NULL)
  {
    assert(tokens_count > 0);
    assert(topics_count > 0);

    counter_token_topic_ = new float[tokens_count_ * topics_count_];
    memset(counter_token_topic_, 0,
           tokens_count_ * topics_count_ * sizeof(float));

    counter_topic_ = new float[topics_count_];
    memset(counter_topic_, 0, topics_count_ * sizeof(float));
  }

  ~ProcessorOutput() {
    delete [] counter_token_topic_;
    delete [] counter_topic_;
  }

  int model_id() const {
    return model_id_;
  }

  int tokens_count() const {
    return tokens_count_;
  }

  int topics_count() const {
    return topics_count_;
  }

  int items_processed() const {
    return items_processed_;
  }

  void set_items_processed(int value) {
    items_processed_ = value;
  }

  const float* counter_token_topic(int token_index) const {
    return &counter_token_topic_[topics_count_ * token_index];
  }

  float* counter_token_topic(int token_index) {
    return &counter_token_topic_[topics_count_ * token_index];
  }

  const float* counter_topic() const {
    return counter_topic_;
  }

  float* counter_topic() {
    return counter_topic_;
  }
  
 private:
  int model_id_;
  int tokens_count_;
  int topics_count_;
  int items_processed_;
  float* counter_token_topic_;
  float* counter_topic_;
};

class Merger : boost::noncopyable {
 public:
  Merger(boost::mutex& merger_queue_lock,
         std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
         ThreadSafeHolder<Generation>& generation,
         ThreadSafeHolder<InstanceSchema>& schema);

  ~Merger() {
    if (thread_.joinable()) {
      thread_.interrupt();
      thread_.join();
    }
  }

  std::shared_ptr<const TokenTopicMatrix> token_topic_matrix(int model_id) const
  {
    return token_topic_matrix_.get(model_id);
  }

 private:
  mutable boost::mutex lock_;
  ThreadSafeCollectionHolder<int, TokenTopicMatrix> token_topic_matrix_;
  ThreadSafeHolder<Generation>& generation_;
  ThreadSafeHolder<InstanceSchema>& schema_;
  
  boost::mutex& merger_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue_; 

  boost::thread thread_;  
  void ThreadFunction();

  void Initialize(int model_id, const Generation& generation, int topics_count);
  void MergeFromQueueAndUpdateMatrix(int model_id);
};
} // namespace topicmd

#endif // TOPICMD_MERGER_
