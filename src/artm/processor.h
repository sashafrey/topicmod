#ifndef ARTM_PROCESSOR_
#define ARTM_PROCESSOR_

#include <memory>
#include <queue>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>

#include "artm/instance_schema.h"
#include "artm/internals.pb.h"
#include "artm/merger.h"
#include "artm/messages.pb.h"
#include "artm/thread_safe_holder.h"

namespace artm { namespace core {

  class Processor : boost::noncopyable {
  public:
    Processor(boost::mutex& processor_queue_lock,
        std::queue<std::shared_ptr<const ProcessorInput> >&  processor_queue,
        boost::mutex& merger_queue_lock,
        std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
        const Merger& merger,
        ThreadSafeHolder<InstanceSchema>& schema) :
      processor_queue_lock_(processor_queue_lock),
      processor_queue_(processor_queue),
      merger_queue_lock_(merger_queue_lock),
      merger_queue_(merger_queue),
      merger_(merger),
      schema_(schema),
      thread_(boost::bind(&Processor::ThreadFunction, this))
    {
    }

    ~Processor();
    void Interrupt();
    void Join();
  private:
    boost::mutex& processor_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorInput> >& processor_queue_;
    boost::mutex& merger_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue_;
    const Merger& merger_;
    ThreadSafeHolder<InstanceSchema>& schema_;

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
       const Flags* stream_flags_;
       const ProcessorInput& processor_input_;
    };

    // Helper class to perform the actual job 
    // (inferring theta distribution or perplexity calculation)
    class ItemProcessor : boost::noncopyable {
     public:
      ItemProcessor(const TokenTopicMatrix& token_topic_matrix, 
                    const google::protobuf::RepeatedPtrField<std::string>& token_dict);
      
      void InferTheta(const ModelConfig& model, 
                      const Item& item, 
                      ProcessorOutput* processor_output, 
                      float* theta_out);
      
      void CalculateScore(const Score& score, 
                          const Item& item, 
                          const float* theta, 
                          double* perplexity, 
                          double* normalizer);
     private:
      const TokenTopicMatrix& token_topic_matrix_;
      const google::protobuf::RepeatedPtrField<std::string>& token_dict_;
    };

    // Helper class to iterate through tokens in one item
    class TokenIterator : boost::noncopyable {
     public:
      enum Mode {
        Known = 1,
        Unknown = 2,
        KnownAndUnknown = 3
      };

      TokenIterator(const google::protobuf::RepeatedPtrField<std::string>& token_dict, 
                    const TokenTopicMatrix& token_topic_matrix, 
                    const Item& item, 
                    const std::string& field_name, 
                    Mode mode = KnownAndUnknown);

     bool Next();
     void Reset();

     const std::string& token() const { return token_; }
     int id() const { return token_id_; }
     int count() const { return count_; }
     TokenWeights weights() const { return token_topic_matrix_.token_weights(id()); }

     private:
      const google::protobuf::RepeatedPtrField<std::string>& token_dict_;
      const TokenTopicMatrix& token_topic_matrix_;
      const Field* field_;
      int token_size_;
      bool iterate_known_;
      bool iterate_unknown_;
      
      // Current state of the iterator
      int token_index_;
      std::string token_;
      int token_id_;
      int count_;
    };
  };
}} // artm/core

#endif // ARTM_PROCESSOR_
