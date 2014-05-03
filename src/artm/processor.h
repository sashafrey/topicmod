// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_PROCESSOR_H_
#define SRC_ARTM_PROCESSOR_H_

#include <memory>
#include <queue>
#include <string>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/bind.hpp"
#include "boost/utility.hpp"

#include "artm/common.h"
#include "artm/instance_schema.h"
#include "artm/internals.pb.h"
#include "artm/merger.h"
#include "artm/messages.pb.h"
#include "artm/thread_safe_holder.h"

#include "artm/regularizer_interface.h"

// include here all files containig regularizers
#include "artm/dirichlet_regularizer_theta.h"

namespace artm {
namespace core {

class Processor : boost::noncopyable {
 public:
  Processor(boost::mutex* processor_queue_lock,
      std::queue<std::shared_ptr<const ProcessorInput> >*  processor_queue,
      boost::mutex* merger_queue_lock,
      std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue,
      const Merger& merger,
      const ThreadSafeHolder<InstanceSchema>& schema) :
    processor_queue_lock_(processor_queue_lock),
    processor_queue_(processor_queue),
    merger_queue_lock_(merger_queue_lock),
    merger_queue_(merger_queue),
    merger_(merger),
    schema_(schema),
    thread_() {
    // Keep this at the last action in constructor.
    // http://stackoverflow.com/questions/15751618/initialize-boost-thread-in-object-constructor
    boost::thread t(&Processor::ThreadFunction, this);
    thread_.swap(t);
  }

  ~Processor();
  void Interrupt();
  void Join();

 private:
  boost::mutex* processor_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorInput> >* processor_queue_;
  boost::mutex* merger_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue_;
  const Merger& merger_;
  const ThreadSafeHolder<InstanceSchema>& schema_;

  boost::thread thread_;
  void ThreadFunction();

  // Helper class to iterate on stream
  class StreamIterator : boost::noncopyable {
   public:
    StreamIterator(const ProcessorInput& processor_input, const std::string stream_name);
    const Item* Next();
    const Item* Current() const;

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
    ItemProcessor(const TopicModel& topic_model,
                  const google::protobuf::RepeatedPtrField<std::string>& token_dict,
                  std::shared_ptr<InstanceSchema> schema);

    void InferTheta(const ModelConfig& model,
                    const Item& item,
                    ModelIncrement* model_increment,
                    float* theta);

    void CalculateScore(const Score& score,
                        const Item& item,
                        const float* theta,
                        double* perplexity,
                        double* normalizer);

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
    TopicWeightIterator GetTopicWeightIterator() const {
      return std::move(topic_model_.GetTopicWeightIterator(id_in_model()));
    }

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


#endif  // SRC_ARTM_PROCESSOR_H_
