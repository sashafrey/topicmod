#include "topicmd/processor.h"

#include "stdlib.h"

namespace topicmd {
  Processor::~Processor() {
    if (thread_.joinable()) {
      thread_.interrupt();
      thread_.join();
    }
  }

  void Processor::Interrupt() {
    thread_.interrupt();
  }

  void Processor::Join() {
    thread_.join();
  }

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
              = merger_.GetLatestTokenTopicMatrix(model_id);
          assert(token_topic_matrix.get() != NULL);
          int tokens_count = token_topic_matrix->tokens_count();
          int topics_count = token_topic_matrix->topics_count();
		      int items_count = part->get_item_count();
          
          // process part and store result in merger queue
          auto po = std::make_shared<ProcessorOutput>();
		      po->set_model_id(model_id);
		      po->set_items_processed(items_count);
		      po->set_topics_count(topics_count);
          for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
            po->mutable_topic_counters()->add_value(0.0f);
          }

		      for (int iToken = 0; iToken < token_topic_matrix->tokens_count(); iToken++) {
			      po->add_token(token_topic_matrix->token(iToken));
			      Counters* counters = po->add_token_counters();
			      for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
				      counters->add_value(0.0f);
			      }
		      }

          for (int item_index = 0;
                item_index < items_count;
                ++item_index)
          {
            std::vector<float> theta(topics_count);
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
              theta[iTopic] = (float)rand() / (float)RAND_MAX;
            }

            // find the id of token in token_topic_matrix
            std::vector<int> this_item_token_id;
            std::vector<float> this_item_token_frequency;
            for (int token_index = 0;
                 token_index < part->get_token_count(item_index);
                 token_index++)
            {
              std::string token = part->get_token(item_index, token_index);
              int token_id = token_topic_matrix->token_id(token);
			        if (token_id < 0) {
				        // Unknown token
				        po->add_discovered_token(token);
			        } else {
				        this_item_token_id.push_back(token_id);
				        this_item_token_frequency.push_back(
					        part->get_token_frequency(item_index, token_index));
			        }
            }

			      int this_item_token_count = this_item_token_id.size();
            std::vector<float> Z(this_item_token_count);
            int numInnerIters = model.inner_iterations_count();
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
                TokenWeights cur_token_weights = token_topic_matrix->token_weights(
                  this_item_token_id[token_index]);
                for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                  curZ += cur_token_weights.at(iTopic) * theta[iTopic];
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
                TokenWeights cur_token_weights = token_topic_matrix->token_weights(
                    this_item_token_id[token_index]);
                float curZ = Z[token_index];
          
                if (curZ > 0) {
                  if (iInnerIter < numInnerIters) {
                    // Normal iteration, updating theta_next
                    for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                      theta_next[iTopic] += n_dw * (cur_token_weights.at(iTopic))
                        * theta[iTopic] / curZ;
                    }
                  } else {
                    // Last iteration, updating final counters
                    Counters* hat_n_wt_cur = po->mutable_token_counters(
						          this_item_token_id[token_index]);
					          Counters* hat_n_t = po->mutable_topic_counters();
                
                    for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                      float val = n_dw * (cur_token_weights.at(iTopic)) *
                          theta[iTopic] / curZ;

					            hat_n_wt_cur->set_value(iTopic, hat_n_wt_cur->value(iTopic) + val);
					            hat_n_t->set_value(iTopic, hat_n_t->value(iTopic) + val);
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
            merger_queue_.push(po);
          }
        });
      }
    }
    catch(boost::thread_interrupted&) {
      return;
    }
  }
} // namespace topicmd
