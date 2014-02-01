#include "merger.h"

#include <algorithm>
#include <iostream>

namespace topicmd {

Merger::Merger(boost::mutex& merger_queue_lock,
               std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
               ThreadSafeHolder<InstanceSchema>& schema) :
    lock_(),
    token_topic_matrix_(lock_),
    schema_(schema),
    merger_queue_lock_(merger_queue_lock),
    merger_queue_(merger_queue),
    thread_(boost::bind(&Merger::ThreadFunction, this))
{
}

void Merger::ThreadFunction() 
{
  try {
    int last_generation_id = -1;
    for (;;)
    {
      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      // 1. Ensure that each model in the schema is initialized
      // ToDo: there is a raise between here and processor. 
      // Processor might ask a model when it is not initialized.
      std::shared_ptr<InstanceSchema> schema = schema_.get();
      std::vector<int> model_ids = schema->get_model_ids();
      std::for_each(model_ids.begin(), model_ids.end(), [&](int model_id) {
        const ModelConfig& model = schema->get_model_config(model_id);

        if (!token_topic_matrix_.has_key(model_id)) {
          auto ttm = std::make_shared<TokenTopicMatrix>(model.topics_count());
          token_topic_matrix_.set(model_id, ttm);
        }
      });
  
      // 2. Merge everything from the queue and update matrix.
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
        auto new_ttm = std::make_shared<TokenTopicMatrix>(*cur_ttm);
        new_ttm->add_items_processed(processor_output->items_processed());
        
        // Add new tokens discovered by processor
        for (int iNewToken = 0; iNewToken < processor_output->discovered_token_size(); ++iNewToken) {
          new_ttm->add_token(processor_output->discovered_token(iNewToken));
        }

        int topics_count = new_ttm->topics_count();

        for (int iToken = 0; iToken < processor_output->token_counters_size(); ++iToken) {
          const Counters& counters = processor_output->token_counters(iToken);
          const std::string& token = processor_output->token(iToken);
          int id = new_ttm->token_id(token);
          for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
            new_ttm->add_token_topic(id, iTopic, counters.value(iTopic));
          }
        }

        token_topic_matrix_.set(model_id, new_ttm);
      }
    }
  }
  catch(boost::thread_interrupted&) {
    return;
  }
}

} // namespace topicmd
