// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/processor.h"

#include <stdlib.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "glog/logging.h"

#include "artm/regularizer_interface.h"
#include "artm/score_calculator_interface.h"

#include "artm/core/protobuf_helpers.h"
#include "artm/core/call_on_destruction.h"
#include "artm/core/helpers.h"
#include "artm/core/instance_schema.h"
#include "artm/core/merger.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace core {

Processor::Processor(ThreadSafeQueue<std::shared_ptr<const ProcessorInput> >*  processor_queue,
                     ThreadSafeQueue<std::shared_ptr<const ModelIncrement> >* merger_queue,
                     const Merger& merger,
                     const ThreadSafeHolder<InstanceSchema>& schema)
    : processor_queue_(processor_queue),
      merger_queue_(merger_queue),
      merger_(merger),
      schema_(schema),
      is_stopping(false),
      thread_() {
  // Keep this at the last action in constructor.
  // http://stackoverflow.com/questions/15751618/initialize-boost-thread-in-object-constructor
  boost::thread t(&Processor::ThreadFunction, this);
  thread_.swap(t);
}

Processor::~Processor() {
  is_stopping = true;
  if (thread_.joinable()) {
    thread_.join();
  }
}

Processor::TokenIterator::TokenIterator(
    const google::protobuf::RepeatedPtrField<std::string>& token_dict,
    const TopicModel& topic_model,
    const Item& item, const std::string& field_name,
    Mode mode)
    : token_dict_(token_dict),
      topic_model_(topic_model),
      field_(nullptr),
      token_size_(0),
      iterate_known_((mode & Mode_Known) != 0),       // NOLINT
      iterate_unknown_((mode & Mode_Unknown) != 0),   // NOLINT
      token_index_(-1),  // handy trick for iterators
      token_(),
      id_in_model_(-1),
      id_in_batch_(-1),
      count_(0) {
  for (int field_index = 0; field_index < item.field_size(); field_index++) {
    if (item.field(field_index).name() == field_name) {
      field_ = &item.field(field_index);
    }
  }

  if (field_ == nullptr) {
    return;
  }

  token_size_ = field_->token_id_size();
}

void Processor::TokenIterator::Reset() {
  token_index_ = -1;  // handy trick for iterators

  // Reset all other data.
  // This makes it less confusing if somebody access this data after Reset() but before Next().
  token_.clear();
  id_in_model_ = -1;
  id_in_batch_ = -1;
  count_ = 0;
}

bool Processor::TokenIterator::Next() {
  if (field_ == nullptr) {
    return false;
  }

  for (;;) {
    token_index_++;  // handy trick pays you back!

    if (token_index_ >= token_size_) {
      // reached the end of the stream
      return false;
    }

    id_in_batch_ = field_->token_id(token_index_);
    token_ = token_dict_.Get(id_in_batch_);
    count_ = field_->token_count(token_index_);
    id_in_model_ = topic_model_.has_token(token_) ? topic_model_.token_id(token_) : -1;

    if (iterate_known_ && (id_in_model_ >= 0)) {
      return true;
    }

    if (iterate_unknown_ && (id_in_model_ < 0)) {
      return true;
    }
  }

  return false;
}

TopicWeightIterator Processor::TokenIterator::GetTopicWeightIterator() const {
  return std::move(topic_model_.GetTopicWeightIterator(id_in_model()));
}

Processor::ItemProcessor::ItemProcessor(
    const TopicModel& topic_model,
    const google::protobuf::RepeatedPtrField<std::string>& token_dict,
    std::shared_ptr<InstanceSchema> schema)
    : topic_model_(topic_model),
      token_dict_(token_dict),
      schema_(schema) {}

void Processor::ItemProcessor::InferTheta(const ModelConfig& model,
                                          const Item& item,
                                          ModelIncrement* model_increment,
                                          bool update_model,
                                          float* theta) {
  int topic_size = topic_model_.topic_size();

  // find the id of token in topic_model
  std::vector<int> token_id;
  std::vector<float> token_count;
  std::vector<TopicWeightIterator> token_weights;
  std::vector<float> z_normalizer;
  int known_tokens_count = 0;
  TokenIterator iter(token_dict_, topic_model_, item, model.field_name(),
                      TokenIterator::Mode_Known);

  while (iter.Next()) {
    token_id.push_back(iter.id_in_batch());
    token_count.push_back(static_cast<float>(iter.count()));
    token_weights.push_back(iter.GetTopicWeightIterator());
    z_normalizer.push_back(0.0f);
    known_tokens_count++;
  }

  if (known_tokens_count == 0) {
    return;
  }

  int inner_iters_count = model.inner_iterations_count();
  for (int inner_iter = 0; inner_iter <= inner_iters_count; inner_iter++) {
    // 1. Find Z
    for (int token_index = 0;
          token_index < known_tokens_count;
          ++token_index) {
      float cur_z = 0.0f;
      TopicWeightIterator topic_iter = token_weights[token_index];
      topic_iter.Reset();
      while (topic_iter.NextNonZeroTopic() < topic_size) {
        cur_z += topic_iter.Weight() * theta[topic_iter.TopicIndex()];
      }

      z_normalizer[token_index] = cur_z;
    }

    // 2. Find new theta (or store results if on the last iteration)
    std::vector<float> theta_next(topic_size);
    memset(&theta_next[0], 0, topic_size * sizeof(float));
    for (int token_index = 0;
          token_index < known_tokens_count;
          ++token_index) {
      float n_dw = token_count[token_index];
      TopicWeightIterator topic_iter = token_weights[token_index];
      float curZ = z_normalizer[token_index];

      if (curZ > 0) {
        // updating theta_next
        topic_iter.Reset();
        while (topic_iter.NextNonZeroTopic() < topic_size) {
          theta_next[topic_iter.TopicIndex()] +=
            n_dw * topic_iter.Weight() * theta[topic_iter.TopicIndex()] / curZ;
        }

        if ((inner_iter == inner_iters_count) &&
            (model_increment != nullptr) &&
             update_model) {
          // Last iteration, updating final counters
          FloatArray* hat_n_wt_cur = model_increment->mutable_token_increment(
            token_id[token_index]);

          topic_iter.Reset();
          while (topic_iter.NextNonZeroTopic() < topic_size) {
            float val = n_dw * topic_iter.Weight() * theta[topic_iter.TopicIndex()] / curZ;
            hat_n_wt_cur->set_value(
              topic_iter.TopicIndex(),
              hat_n_wt_cur->value(topic_iter.TopicIndex()) + val);
          }
        }
      }
    }

    if (inner_iter == inner_iters_count) {
      // inner_iter goes from 0 to inner_iters_count inclusively.
      // The goal of this "last iteration" is to update model_increment.
      // As soon as model_increment is updated, we should exit.
      // This will save redundant calculations, and prevent
      // calling RegularizeTheta with too large inner_iter.
      break;
    }

    // 3. The following block of code makes the regularization of theta_next
    auto reg_names = model.regularizer_name();
    auto reg_tau = model.regularizer_tau();
    for (auto reg_name_iterator = reg_names.begin(); reg_name_iterator != reg_names.end();
      reg_name_iterator++) {
      auto regularizer = schema_->regularizer(reg_name_iterator->c_str());
      if (regularizer != nullptr) {
        auto tau_index = reg_name_iterator - reg_names.begin();
        double tau = reg_tau.Get(tau_index);
        bool retval = regularizer->RegularizeTheta(item, &theta_next, topic_size, inner_iter, tau);
        if (!retval) {
          LOG(ERROR) << "Problems with type or number of parameters in Theta regularizer <" <<
            reg_name_iterator->c_str() <<
            ">. On this iteration this regularizer was turned off.\n";
        }
      } else {
        LOG(ERROR) << "Theta Regularizer with name <" << reg_name_iterator->c_str() <<
          "> does not exist.";
      }
    }

    // Normalize theta_next.
    for (int i = 0; i < static_cast<int>(theta_next.size()); ++i) {
      if (theta_next[i] < 0) {
        theta_next[i] = 0;
      }
    }

    float sum = 0.0f;
    for (int topic_index = 0; topic_index < topic_size; ++topic_index)
      sum += theta_next[topic_index];

    for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
      theta[topic_index] = (sum > 0) ? (theta_next[topic_index] / sum) : 0.0f;
    }
  }
}

Processor::StreamIterator::StreamIterator(const ProcessorInput& processor_input)
    : items_count_(processor_input.batch().item_size()),
      item_index_(-1),  // // handy trick for iterators
      stream_flags_(nullptr),
      processor_input_(processor_input) {
}

Processor::StreamIterator::StreamIterator(const ProcessorInput& processor_input,
                                          const std::string& stream_name)
    : items_count_(processor_input.batch().item_size()),
      item_index_(-1),  // // handy trick for iterators
      stream_flags_(nullptr),
      processor_input_(processor_input) {
  int index_of_stream = repeated_field_index_of(processor_input.stream_name(), stream_name);

  if (index_of_stream == -1) {
    // log a warning and process all documents from the stream
    LOG(WARNING) << "Stream " << stream_name << " does not exist in the batch.";
  } else {
    stream_flags_ = &processor_input.stream_mask(index_of_stream);
  }
}

const Item* Processor::StreamIterator::Next() {
  for (;;) {
    item_index_++;  // handy trick pays you back!

    if (item_index_ >= items_count_) {
      // reached the end of the stream
      break;
    }

    if (!stream_flags_ || stream_flags_->value(item_index_)) {
      // found item that is included in the stream
      break;
    }
  }

  return Current();
}

const Item* Processor::StreamIterator::Current() const {
  if (item_index_ >= items_count_)
    return nullptr;

  return &(processor_input_.batch().item(item_index_));
}

bool Processor::StreamIterator::InStream(const std::string& stream_name) {
  if (item_index_ >= items_count_)
    return false;

  int index_of_stream = repeated_field_index_of(processor_input_.stream_name(), stream_name);
  if (index_of_stream == -1) {
    return true;
  }

  return processor_input_.stream_mask(index_of_stream).value(item_index_);
}

bool Processor::StreamIterator::InStream(int stream_index) {
  if (stream_index == -1)
    return true;

  assert(stream_index >= 0 && stream_index < processor_input_.stream_name_size());

  if (item_index_ >= items_count_)
    return false;

  return processor_input_.stream_mask(stream_index).value(item_index_);
}

void Processor::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "Processor thread");
    LOG(INFO) << "Processor thread started";
    int pop_retries = 0;
    const int pop_retries_max = 20;
    for (;;) {
      if (is_stopping) {
        LOG(INFO) << "Processor thread stopped";
        break;
      }

      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      std::shared_ptr<const ProcessorInput> part;
      if (!processor_queue_->try_pop(&part)) {
        pop_retries++;
        LOG_IF(INFO, pop_retries == pop_retries_max) << "No data in processing queue, waiting...";
        continue;
      }

      LOG_IF(INFO, pop_retries >= pop_retries_max) << "Processing queue has data, processing started";
      pop_retries = 0;

      std::shared_ptr<InstanceSchema> schema = schema_.get();
      std::vector<ModelName> model_names = schema->GetModelNames();
      std::for_each(model_names.begin(), model_names.end(), [&](ModelName model_name) {
        const ModelConfig& model = schema->model_config(model_name);

        // do not process disabled models.
        if (!model.enabled()) return;  // return from lambda; goes to next step of std::for_each

        // Find and save to the variable the index of model stream in the part->stream_name() list.
        int model_stream_index = repeated_field_index_of(part->stream_name(), model.stream_name());

        std::map<ScoreName, std::shared_ptr<Score>> score_container;
        for (int score_index = 0; score_index < model.score_name_size(); ++score_index) {
          const ScoreName& score_name = model.score_name(score_index);
          auto score_calc = schema->score_calculator(score_name);
          if (score_calc == nullptr) {
            LOG(ERROR) << "Unable to find score calculator '" << score_name << "', referenced by "
                        << "model " << model.name() << ".";
            continue;
          }

          if (!score_calc->is_cumulative())
            continue;  // skip all non-cumulative scores

          score_container.insert(std::make_pair(score_name, score_calc->CreateScore()));
        }

        std::shared_ptr<ModelIncrement> model_increment = std::make_shared<ModelIncrement>();
        model_increment->add_batch_uuid(part->batch_uuid());
        call_on_destruction c([&]() {
          merger_queue_->push(model_increment);
        });

        // find cache
        const DataLoaderCacheEntry* cache = nullptr;
        for (int i = 0; i < part->cached_theta_size(); ++i) {
          if ((part->cached_theta(i).batch_uuid() == part->batch_uuid()) &&
              (part->cached_theta(i).model_name() == model_name)) {
            cache = &part->cached_theta(i);
          }
        }

        std::shared_ptr<const TopicModel> topic_model = merger_.GetLatestTopicModel(model_name);
        assert(topic_model.get() != nullptr);

        int topic_size = topic_model->topic_size();
        assert(topic_size > 0);

        // process part and store result in merger queue
        model_increment->set_model_name(model_name);
        model_increment->set_topics_count(topic_size);

        for (int token_index = 0; token_index < part->batch().token_size(); ++token_index) {
          std::string token = part->batch().token(token_index);
          model_increment->add_token(token);
          FloatArray* counters = model_increment->add_token_increment();
          for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
            counters->add_value(0.0f);
          }

          if (!topic_model->has_token(token)) {
            model_increment->add_discovered_token(token);
          }
        }

        ItemProcessor item_processor(*topic_model, part->batch().token(), schema_.get());
        StreamIterator iter(*part);
        while (iter.Next() != nullptr) {
          const Item* item = iter.Current();

          // Initialize theta (either assign random values or reuse values from previous iteration)
          std::vector<float> theta(topic_size);
          int index_of_item = -1;
          if ((cache != nullptr) && model.reuse_theta()) {
            index_of_item = repeated_field_index_of(cache->item_id(), item->id());
          }

          if ((index_of_item != -1) && model.reuse_theta()) {
            const FloatArray& old_thetas = cache->theta(index_of_item);
            for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
              theta[topic_index] = old_thetas.value(topic_index);
            }
          } else {
            for (int iTopic = 0; iTopic < topic_size; ++iTopic) {
              theta[iTopic] = ThreadSafeRandom::singleton().GenerateFloat();
            }
          }

          bool update_model = iter.InStream(model_stream_index);
          item_processor.InferTheta(model, *item, model_increment.get(), update_model, &theta[0]);

          // Update theta cache
          model_increment->add_item_id(item->id());
          FloatArray* cached_theta = model_increment->add_theta();
          for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
            cached_theta->add_value(theta[topic_index]);
          }

          // Calculate all requested scores (such as perplexity)
          for (auto score_iter = score_container.begin();
               score_iter != score_container.end();
               ++score_iter) {
            const ScoreName& score_name = score_iter->first;
            std::shared_ptr<Score> score = score_iter->second;
            auto score_calc = schema->score_calculator(score_name);

            if (!iter.InStream(score_calc->stream_name())) continue;

            score_calc->AppendScore(*item, part->batch().token(), *topic_model, theta,
                                    score.get());
          }
        }

        for (auto score_iter = score_container.begin();
             score_iter != score_container.end();
             ++score_iter) {
          model_increment->add_score_name(score_iter->first);
          model_increment->add_score(score_iter->second->SerializeAsString());
        }
      });

      // Wait until merger queue has space for a new element
      int merger_queue_max_size = schema_.get()->config().merger_queue_max_size();

      int push_retries = 0;
      const int push_retries_max = 50;

      for (;;) {
        if (merger_queue_->size() < merger_queue_max_size)
          break;

        push_retries++;
        LOG_IF(WARNING, push_retries == push_retries_max) << "Merger queue is full, waiting...";
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
      }

      LOG_IF(WARNING, push_retries >= push_retries_max) << "Merger queue is healthy again";

      // Here call_in_destruction will enqueue processor output into the merger queue.
    }
  }
  catch(boost::thread_interrupted&) {
    LOG(WARNING) << "thread_interrupted exception in Processor::ThreadFunction() function";
    return;
  } catch(...) {
    LOG(FATAL) << "Fatal exception in Processor::ThreadFunction() function";
    throw;
  }
}

}  // namespace core
}  // namespace artm
