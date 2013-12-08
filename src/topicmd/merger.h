#ifndef TOPICMD_MERGER_
#define TOPICMD_MERGER_

#include "assert.h"

#include <map>
#include <memory>
#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/generation.h"
#include "topicmd/thread_safe_holder.h"
#include "topicmd/token_topic_matrix.h"

namespace topicmd {

class ProcessorOutput : boost::noncopyable {
 public:
  ProcessorOutput(int tokens_count, int topics_count) :
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

  float* counter_token_topic(int token_index) {
    return &counter_token_topic_[topics_count_ * token_index];
  }

  float* counter_topic() {
    return counter_topic_;
  }
  
 private:
  int tokens_count_;
  int topics_count_;
  float* counter_token_topic_;
  float* counter_topic_;
};

class Merger : boost::noncopyable {
 public:
  Merger(const std::shared_ptr<const Generation>& generation,
         int topics_count) :
      lock_(),
      token_topic_matrix_(lock_),
      generation_(generation)
  {
    auto ttm = std::make_shared<TokenTopicMatrix>();
    generation_->InvokeOnEachPartition(
        [&](std::shared_ptr<const Partition> part) {
          auto tokens = part->get_tokens();
          for (auto iter = tokens.begin();
               iter != tokens.end(); iter++)
          {
            ttm->add_token(*iter);
          }
        });

    ttm->Initialize(topics_count);
    token_topic_matrix_.set(ttm);
  }

  std::shared_ptr<const TokenTopicMatrix> token_topic_matrix() const
  {
    return token_topic_matrix_.get();
  }
      
  private:
    mutable boost::mutex lock_;
    ThreadSafeHolder<TokenTopicMatrix> token_topic_matrix_;
    std::shared_ptr<const Generation> generation_;
  };
} // namespace topicmd

#endif // TOPICMD_MERGER_
