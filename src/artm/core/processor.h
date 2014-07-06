// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_PROCESSOR_H_
#define SRC_ARTM_CORE_PROCESSOR_H_

#include <atomic>
#include <memory>
#include <string>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/bind.hpp"
#include "boost/utility.hpp"

#include "artm/messages.pb.h"

#include "artm/core/common.h"
#include "artm/core/internals.pb.h"
#include "artm/core/thread_safe_holder.h"

namespace artm {
namespace core {

class InstanceSchema;
class Merger;
class TopicModel;
class TopicWeightIterator;

class Processor : boost::noncopyable {
 public:
  Processor(ThreadSafeQueue<std::shared_ptr<const ProcessorInput> >*  processor_queue,
            ThreadSafeQueue<std::shared_ptr<const ModelIncrement> >* merger_queue,
            const Merger& merger,
            const ThreadSafeHolder<InstanceSchema>& schema);

  ~Processor();

 private:
  ThreadSafeQueue<std::shared_ptr<const ProcessorInput> >* processor_queue_;
  ThreadSafeQueue<std::shared_ptr<const ModelIncrement> >* merger_queue_;
  const Merger& merger_;
  const ThreadSafeHolder<InstanceSchema>& schema_;

  mutable std::atomic<bool> is_stopping;
  boost::thread thread_;

  void ThreadFunction();

  // Helper class to iterate on stream
  class StreamIterator : boost::noncopyable {
   public:
    // Iterates on a global stream (all items in the batch)
    explicit StreamIterator(const ProcessorInput& processor_input);

    // Iterates on items from a specific stream
    explicit StreamIterator(const ProcessorInput& processor_input, const std::string& stream_name);

    const Item* Next();
    const Item* Current() const;

    // Checks whether Current() item belongs to a specific stream
    bool InStream(const std::string& stream_name);
    bool InStream(int stream_index);

   private:
    int items_count_;
    int item_index_;
    const Mask* stream_flags_;
    const ProcessorInput& processor_input_;
  };

  // Helper class to perform the actual job
  // (inferring theta distribution or perplexity calculation)
  class ItemProcessor : boost::noncopyable {
   public:
    explicit ItemProcessor(const TopicModel& topic_model,
                           const google::protobuf::RepeatedPtrField<std::string>& token_dict,
                           std::shared_ptr<InstanceSchema> schema);

    void InferTheta(const ModelConfig& model,
                    const Item& item,
                    ModelIncrement* model_increment,
                    bool update_model,
                    float* theta);

    void CalculateScore(const Score& score,
                        const Item& item,
                        const float* theta,
                        double* perplexity,
                        double* normalizer,
                        int* zero_words);

   private:
    const TopicModel& topic_model_;
    const google::protobuf::RepeatedPtrField<std::string>& token_dict_;
    std::shared_ptr<InstanceSchema> schema_;
  };

  // Helper class to iterate through tokens in one item
  class TokenIterator : boost::noncopyable {
   public:
    enum Mode {
      Mode_Known = 1,
      Mode_Unknown = 2,
      Mode_KnownAndUnknown = 3
    };

    TokenIterator(const google::protobuf::RepeatedPtrField<std::string>& token_dict,
                  const TopicModel& topic_model,
                  const Item& item,
                  const std::string& field_name,
                  Mode mode = Mode_KnownAndUnknown);

    bool Next();
    void Reset();

    const std::string& token() const { return token_; }
    int id_in_model() const { return id_in_model_; }
    int id_in_batch() const { return id_in_batch_; }
    int count() const { return count_; }
    TopicWeightIterator GetTopicWeightIterator() const;

   private:
    const google::protobuf::RepeatedPtrField<std::string>& token_dict_;
    const TopicModel& topic_model_;
    const Field* field_;
    int token_size_;
    bool iterate_known_;
    bool iterate_unknown_;

    // Current state of the iterator
    int token_index_;
    std::string token_;
    int id_in_model_;
    int id_in_batch_;
    int count_;
  };
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_CORE_PROCESSOR_H_
