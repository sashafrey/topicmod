#include "topicmd/processor.h"

#include "stdlib.h"

namespace topicmd {
  void Processor::ThreadFunction() {
    try
    {
      for (;;) {
        // Sleep and check for interrupt.
        // To check for interrupt without sleep,
        // use boost::this_thread::interruption_point()
        // which also throws boost::thread_interrupted
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));

        std::shared_ptr<const Partition> part;
        {
          boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
          if (processor_queue_.empty()) {
            continue;
          }
        
          part = processor_queue_.front();
          processor_queue_.pop();
        }

        std::shared_ptr<InstanceSchema> schema = schema_.get();
        std::vector<int> model_ids = schema->get_model_ids();
        std::for_each(model_ids.begin(), model_ids.end(), [&](int model_id) {
          const ModelConfig& model = schema->get_model_config(model_id);

          // do not process disabled models.
          if (!model.enabled()) return; // return from lambda; goes to next step of std::for_each

          std::shared_ptr<const TokenTopicMatrix> token_topic_matrix
              = merger_.token_topic_matrix(model_id);
          int tokens_count = token_topic_matrix->tokens_count();
          int topics_count = token_topic_matrix->topics_count();

          // do not process "empty" models (no data arrived yet)
          if (tokens_count == 0) return;
      
          // process part and store result in merger queue
          auto processor_output = std::make_shared<ProcessorOutput>(
              model_id, tokens_count, topics_count);

          int item_count = part->get_item_count();
          processor_output->set_items_processed(item_count);
          for (int item_index = 0;
                item_index < item_count;
                ++item_index)
          {
            std::vector<float> theta(topics_count);
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
              theta[iTopic] = (float)rand() / (float)RAND_MAX;
            }

            int this_item_token_count = part->get_token_count(item_index);

            // find the id of token in token_topic_matrix
            std::vector<int> this_item_token_id;
            std::vector<float> this_item_token_frequency;
            this_item_token_id.reserve(this_item_token_count);
            this_item_token_frequency.reserve(this_item_token_count);
            for (int token_index = 0;
                  token_index < this_item_token_count;
                  token_index++)
            {
              std::string token = part->get_token(item_index, token_index);
              int token_id = token_topic_matrix->token_id(token);
              assert(token_id >= 0);
              this_item_token_id.push_back(token_id);
              this_item_token_frequency.push_back(
                  part->get_token_frequency(item_index, token_index));
            }

            std::vector<float> Z(this_item_token_count);
            int numInnerIters = 10; // ToDo
            for (int iInnerIter = 0;
                  iInnerIter <= numInnerIters;
                  iInnerIter++)
            {
              // 1. Find Z
              for (int token_index = 0;
                    token_index < this_item_token_count;
                    ++token_index)
              {
                float curZ = 0.0f;
                float* cur_token_topics = token_topic_matrix->token_topics(
                    this_item_token_id[token_index]);
                for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                  curZ += cur_token_topics[iTopic] * theta[iTopic];
                }

                Z[token_index] = curZ;
              }

              // 2. Find new theta (or store results if on the last iteration)
              std::vector<float> theta_next(topics_count);
              memset(&theta_next[0], 0, topics_count * sizeof(float));
              for (int token_index = 0;
                    token_index < this_item_token_count;
                    ++token_index)
              {
                float n_dw = this_item_token_frequency[token_index];
                float* cur_token_topics = token_topic_matrix->token_topics(
                    this_item_token_id[token_index]);
                float curZ = Z[token_index];
          
                if (curZ > 0) {
                  if (iInnerIter < numInnerIters) {
                    // Normal iteration, updating theta_next
                    for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                      theta_next[iTopic] += n_dw * cur_token_topics[iTopic]
                          * theta[iTopic] / curZ;
                    }
                  } else {
                    // Last iteration, updating final counters
                    float* hat_n_wt_cur =
                        processor_output->counter_token_topic(
                            this_item_token_id[token_index]);
                    float* hat_n_t = processor_output->counter_topic();
                
                    for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                      float val = n_dw * cur_token_topics[iTopic] *
                          theta[iTopic] / curZ;
                      hat_n_wt_cur[iTopic] += val;
                      hat_n_t[iTopic] += val;
                    }
                  }
                }
              }                

              if (iInnerIter < numInnerIters) {
                for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                  theta[iTopic] = theta_next[iTopic];
                }
              }
            }
          }

          {
            boost::lock_guard<boost::mutex> guard(merger_queue_lock_);
            merger_queue_.push(processor_output);
          }
        });
      }
    }
    catch(boost::thread_interrupted&) {
      return;
    }
  }
} // namespace topicmd
