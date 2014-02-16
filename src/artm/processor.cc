#include "artm/processor.h"
#include "artm/protobuf_helpers.h"
#include "artm/data_loader.h"
#include "artm/call_on_destruction.h"

#include "stdlib.h"

namespace artm { namespace core {
  
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

 
  Processor::TokenIterator::TokenIterator(const google::protobuf::RepeatedPtrField<std::string>& token_dict, 
                const TokenTopicMatrix& token_topic_matrix, 
                const Item& item, 
                const std::string& field_name, 
                Mode mode) :
      token_dict_(token_dict),
      token_topic_matrix_(token_topic_matrix),
      field_(nullptr),
      token_size_(0),
      iterate_known_(mode & Known),
      iterate_unknown_(mode & Unknown),
      token_index_(-1), // dirty trick (!!!)
      token_(),
      token_id_(-1)
  {
    for (int iField = 0; iField < item.field_size(); iField++) {
      if (item.field(iField).field_name() == field_name) {
        field_ = &item.field(iField);
      }
    }

    if (field_ == nullptr) {
      return;
    }

    token_size_ = field_->token_id_size();
  }

  void Processor::TokenIterator::Reset() { token_index_ = -1; } // dirty trick, again (!!!)

  bool Processor::TokenIterator::Next() {
    if (field_ == nullptr) {
      return false;
    }

    for (;;) {
      token_index_++; // dirty trick pays you back! (!!!)

      if (token_index_ >= token_size_) 
        return false; // reached the end of the stream

      token_ = token_dict_.Get(field_->token_id(token_index_));
      count_ = field_->token_count(token_index_);
      token_id_ = token_topic_matrix_.token_id(token_);

      if (iterate_known_ && (token_id_ >= 0)) {
        
        return true;
      }

      if (iterate_unknown_ && (token_id_ < 0)) {
        return true;
      }
    }

    return false;
  }

    
  Processor::ItemProcessor::ItemProcessor(const TokenTopicMatrix& token_topic_matrix, 
                                          const google::protobuf::RepeatedPtrField<std::string>& token_dict) :
      token_topic_matrix_(token_topic_matrix),
      token_dict_(token_dict)
  {
  }

  void Processor::ItemProcessor::InferTheta(const ModelConfig& model, 
                                            const Item& item,  
                                            ProcessorOutput* processor_output, 
                                            float* theta) 
  {
    int topics_count = token_topic_matrix_.topics_count();

    if (processor_output != nullptr) {
      processor_output->set_items_processed(
        processor_output->items_processed() + 1);
    }

    if (processor_output != nullptr) {
      // Process unknown tokens (if any)
      TokenIterator iter(token_dict_, token_topic_matrix_, item, model.field_name(), TokenIterator::Mode::Unknown);
      while (iter.Next()) {
        processor_output->add_discovered_token(iter.token());
      }
    }

    // find the id of token in token_topic_matrix
    std::vector<int> token_id;
    std::vector<float> token_count;
    std::vector<TokenWeights> token_weights;
    std::vector<float> Z;
    int known_tokens_count = 0;
    TokenIterator iter(token_dict_, token_topic_matrix_, item, model.field_name(), TokenIterator::Mode::Known);
    while (iter.Next()) {
      token_id.push_back(iter.id());
      token_count.push_back(iter.count());
      token_weights.push_back(iter.weights());
      Z.push_back(0.0f);
      known_tokens_count++;
    }

    //if (known_tokens_count == 0) {
    //  return;
    //}

    int numInnerIters = model.inner_iterations_count();
    for (int iInnerIter = 0;
          iInnerIter <= numInnerIters;
          iInnerIter++)
    {
      // 1. Find Z
      for (int token_index = 0;
            token_index < known_tokens_count;
            ++token_index)
      {
        float curZ = 0.0f;
        TokenWeights cur_token_weights = token_weights[token_index];
        for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
          curZ += cur_token_weights.at(iTopic) * theta[iTopic];
        }

        Z[token_index] = curZ;
      }

      // 2. Find new theta (or store results if on the last iteration)
      std::vector<float> theta_next(topics_count);
      memset(&theta_next[0], 0, topics_count * sizeof(float));
      for (int token_index = 0;
            token_index < known_tokens_count;
            ++token_index)
      {
        float n_dw = token_count[token_index];
        TokenWeights cur_token_weights = token_weights[token_index];
        float curZ = Z[token_index];
          
        if (curZ > 0) {
          // updating theta_next
          for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
            float w = cur_token_weights.at(iTopic);
            theta_next[iTopic] += n_dw * w * theta[iTopic] / curZ;
          }

          if ((iInnerIter == numInnerIters) && (processor_output != nullptr)) {
            // Last iteration, updating final counters
            Counters* hat_n_wt_cur = processor_output->mutable_token_counters(
              token_id[token_index]);
            Counters* hat_n_t = processor_output->mutable_topic_counters();
                
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
              float val = n_dw * (cur_token_weights.at(iTopic)) *
                  theta[iTopic] / curZ;

              hat_n_wt_cur->set_value(iTopic, hat_n_wt_cur->value(iTopic) + val);
              hat_n_t->set_value(iTopic, hat_n_t->value(iTopic) + val);
            }
          }

          if (iInnerIter == numInnerIters) {
            // Last iteration, reporting theta vector

            // Normalize theta_next. For normal iterations this is handled by curZ value.
            float sum = 0.0f; 
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) 
              sum += theta_next[iTopic];
            
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
              theta[iTopic] = (sum > 0) ? (theta_next[iTopic] / sum) : 0.0f;
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

  void Processor::ItemProcessor::CalculateScore(const Score& score, const Item& item, const float* theta, double* perplexity, double* normalizer) {
    TokenIterator iter(token_dict_, token_topic_matrix_, item, score.field_name(), TokenIterator::Mode::Known);
    while(iter.Next()) {
      TokenWeights weights = iter.weights();
      float sum = 0.0f;
      for (int iTopic = 0; iTopic < token_topic_matrix_.topics_count(); ++iTopic) {
        sum += theta[iTopic] * weights.at(iTopic);
      }

      (*normalizer) += iter.count();
      (*perplexity) += iter.count() * log(sum);
    }
  }

  Processor::StreamIterator::StreamIterator(const ProcessorInput& processor_input, const std::string stream_name) : 
      items_count_(processor_input.batch().item_size()), 
      item_index_(-1), // dirty trick (!!!)
      stream_flags_(nullptr), 
      processor_input_(processor_input)
  {
    int index_of_stream = repeated_field_index_of(processor_input.stream_name(), stream_name);

    if (index_of_stream == -1) {
      // log a warning and process all documents from the stream
    } else {
      stream_flags_ = &processor_input.stream_flags(index_of_stream);
    }
  }

  const Item* Processor::StreamIterator::Next() {
    for (;;) {
      item_index_++; // dirty trick pays you back! (!!!)

      if (item_index_ >= items_count_) 
        break; // reached the end of the stream

      if (!stream_flags_ || stream_flags_->value(item_index_)) {
        break; // found item that is included in the stream
      }
    }

    return Current();
  }

  const Item* Processor::StreamIterator::Current() const {
    if (item_index_ >= items_count_)
      return nullptr;

    return &(processor_input_.batch().item(item_index_));
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

        std::shared_ptr<const ProcessorInput> part;
        {
          boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
          if (processor_queue_.empty()) {
            continue;
          }
        
          part = processor_queue_.front();
          processor_queue_.pop();
        }

        std::shared_ptr<ProcessorCacheEntry> cache_new = std::make_shared<ProcessorCacheEntry>();
        cache_new->set_uuid(part->uuid());
        helpers::call_on_destruction c([&]() {
          // Callback to DataLoader
          auto data_loader = DataLoaderManager::singleton().Get(part->data_loader_id());
          if (data_loader != nullptr) {
            data_loader->Callback(cache_new);
          }
        });

        const ProcessorCacheEntry* cache_old = part->has_cache() ? &part->cache() : nullptr;

        std::shared_ptr<InstanceSchema> schema = schema_.get();
        std::vector<int> model_ids = schema->get_model_ids();
        std::for_each(model_ids.begin(), model_ids.end(), [&](int model_id) {
          const ModelConfig& model = schema->get_model_config(model_id);
          
          // do not process disabled models.
          if (!model.enabled()) return; // return from lambda; goes to next step of std::for_each

          // find cache
          const ProcessorOutput* old_cache_this_model = nullptr;
          if (cache_old != nullptr) {
            for (int i = 0; i < cache_old->output_size(); ++i) {
              if (cache_old->output(i).model_id() == model_id) {
                old_cache_this_model = &cache_old->output(i);
              }
            }            
          }

          std::shared_ptr<const TokenTopicMatrix> token_topic_matrix
              = merger_.GetLatestTokenTopicMatrix(model_id);
          assert(token_topic_matrix.get() != nullptr);
          
          int topics_count = token_topic_matrix->topics_count();
          int items_count = part->batch().item_size();

          assert(topics_count > 0);
          
          // TODO: if (cache_old != nullptr), deduct old values

          // process part and store result in merger queue
          auto po = std::make_shared<ProcessorOutput>();
          po->set_model_id(model_id);
          po->set_items_processed(0);

          // Prepare score vector
          for (int iScore = 0; iScore < model.score_size(); ++iScore) {
            po->add_score(0.0);
            po->add_score_norm(0.0);
          }

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

          ItemProcessor item_processor(*token_topic_matrix, part->batch().token());
          StreamIterator iter(*part, model.stream_name());
          while (iter.Next() != nullptr) 
          {
            // ToDo: add an option to always start with random iteration!
            const Item* item = iter.Current();

            // ToDo: if (cache_old != nullptr), use it as a starting iteration.
            std::vector<float> theta(topics_count);
            int index_of_item = -1;
            if (old_cache_this_model != nullptr) {
              index_of_item = repeated_field_index_of(old_cache_this_model->item_id(), item->id());
            }

            if (index_of_item != -1) {
              const Counters& old_thetas = old_cache_this_model->theta(index_of_item);
              for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                theta[iTopic] = old_thetas.value(iTopic);
              }              
            } else {
              for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
                theta[iTopic] = (float)rand() / (float)RAND_MAX;
              }
            }

            item_processor.InferTheta(model, *item, po.get(), &theta[0]);

            // Cache theta for the next iteration
            po->add_item_id(item->id());
            Counters* cached_theta = po->add_theta();
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
              cached_theta->add_value(theta[iTopic]);
            }
          }

          // Calculate all requested scores (such as perplexity)
          for (int iScore = 0; iScore < model.score_size(); ++iScore) {
            const Score& score = model.score(iScore);
            
            // Perplexity is so far the only score that Processor know how to calculate.
            if (score.type() != Score_Type_Perplexity) 
              continue;

            std::vector<float> theta_vec;
            theta_vec.resize(topics_count);
            float* theta = &theta_vec[0];
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
              theta[iTopic] = (float)rand() / (float)RAND_MAX;
            }

            double perplexity_score = 0.0;
            double perplexity_norm = 0.0;
            StreamIterator test_iter(*part, score.stream_name());
            ItemProcessor test_item_processor(*token_topic_matrix, part->batch().token());
            while (test_iter.Next() != nullptr) 
            {
              const Item* item = test_iter.Current();
              test_item_processor.InferTheta(model, *item, nullptr, theta);
              test_item_processor.CalculateScore(score, *item, theta, &perplexity_score, &perplexity_norm);
            }

            po->set_score(iScore, po->score(iScore) + perplexity_score);
            po->set_score_norm(iScore, po->score_norm(iScore) + perplexity_norm);
          }

          {
            boost::lock_guard<boost::mutex> guard(merger_queue_lock_);
            merger_queue_.push(po);
          }

          ProcessorOutput* cached_output = cache_new->add_output();
          cached_output->CopyFrom(*po);
        });
      }
    }
    catch(boost::thread_interrupted&) {
      return;
    }
  }
}} // namespace artm/core
