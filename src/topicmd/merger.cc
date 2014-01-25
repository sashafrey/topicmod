#include "merger.h"

#include <algorithm>
#include <iostream>

namespace topicmd {

Merger::Merger(boost::mutex& merger_queue_lock,
               std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
               ThreadSafeHolder<Generation>& generation,
               ThreadSafeHolder<InstanceSchema>& schema) :
    lock_(),
    token_topic_matrix_(lock_),
    generation_(generation),
    schema_(schema),
    merger_queue_lock_(merger_queue_lock),
    merger_queue_(merger_queue),
    thread_(boost::bind(&Merger::ThreadFunction, this))
{
}

void Merger::Initialize(int model_id, 
                        const Generation& generation,
                        int topics_count)
{
  auto ttm = std::make_shared<TokenTopicMatrix>();
  generation.InvokeOnEachPartition(
      [&](std::shared_ptr<const Partition> part) {
        auto tokens = part->get_tokens();
        for (auto iter = tokens.begin();
             iter != tokens.end(); iter++)
        {
          ttm->add_token(*iter);
        }
      });

  bool as_random = true;
  ttm->Initialize(topics_count, as_random);
  token_topic_matrix_.set(model_id, ttm);
}

void Merger::ThreadFunction() 
{
  try {
    int last_generation_id = -1;
    for (;;)
    {
      auto last_generation = generation_.get();

      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      std::shared_ptr<InstanceSchema> schema = schema_.get();
      std::vector<int> model_ids = schema->get_model_ids();
      std::for_each(model_ids.begin(), model_ids.end(), [&](int model_id) {
        const ModelConfig& model = schema->get_model_config(model_id);
          
        if (last_generation->get_id() != last_generation_id) {
          // If generation was updated, check new tokens for the model.
          Initialize(model_id, *last_generation, model.topics_count());
        }
  
        // 2. Merge everything from the queue and update matrix.
        MergeFromQueueAndUpdateMatrix(model_id);
      });

      last_generation_id = last_generation->get_id();
    }
  }
  catch(boost::thread_interrupted&) {
    return;
  }
}

void Merger::MergeFromQueueAndUpdateMatrix(int model_id) {
  for (;;) {
    std::shared_ptr<const ProcessorOutput> processor_output;
    {
      boost::lock_guard<boost::mutex> guard(merger_queue_lock_);
      if (merger_queue_.empty()) {
        break;
      }

      processor_output = merger_queue_.front();
      merger_queue_.pop();
    }

    int model_id = processor_output->model_id();
    auto cur_ttm = token_topic_matrix_.get(model_id);
    auto new_ttm = std::make_shared<TokenTopicMatrix>();
    new_ttm->Initialize(*cur_ttm);
    new_ttm->add_items_processed(processor_output->items_processed());
    float* target = new_ttm->token_topics(0);
    int token_size = cur_ttm->tokens_count();
    int topic_size = cur_ttm->topics_count();
    std::vector<float> topics(topic_size, 0);

    const float* source = processor_output->counter_token_topic(0);
    for (int i = 0; i < token_size * topic_size; i++) {
      target[i] += source[i];
    }

    for (int i = 0; i < topic_size; i++) {
      topics[i] += processor_output->counter_topic()[i];
    }

    // divide phase
    for (int iToken = 0; iToken < token_size; ++iToken) {
      for (int iTopic = 0; iTopic < topic_size; ++iTopic) {
        target[iToken * topic_size + iTopic] /= topics[iTopic];
      }
    }

    token_topic_matrix_.set(model_id, new_ttm);
  }
}


} // namespace topicmd
