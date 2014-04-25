// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/processor.h"

#include <stdlib.h>
#include <string>
#include <vector>

#include "glog/logging.h"

#include "artm/protobuf_helpers.h"
#include "artm/call_on_destruction.h"
#include "artm/helpers.h"

namespace artm {
namespace core {

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
      token_id_(-1) {
  for (int field_index = 0; field_index < item.field_size(); field_index++) {
    if (item.field(field_index).field_name() == field_name) {
      field_ = &item.field(field_index);
    }
  }

  if (field_ == nullptr) {
    return;
  }

  token_size_ = field_->token_id_size();
}

void Processor::TokenIterator::Reset() { token_index_ = -1; }  // handy trick for iterators

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

    token_ = token_dict_.Get(field_->token_id(token_index_));
    count_ = field_->token_count(token_index_);
    token_id_ = topic_model_.has_token(token_) ? topic_model_.token_id(token_) : -1;

    if (iterate_known_ && (token_id_ >= 0)) {
      return true;
    }

    if (iterate_unknown_ && (token_id_ < 0)) {
      return true;
    }
  }

  return false;
}


Processor::ItemProcessor::ItemProcessor(
    const TopicModel& topic_model,
    const google::protobuf::RepeatedPtrField<std::string>& token_dict,
    std::shared_ptr<std::map<std::string, std::shared_ptr<RegularizerInterface> >> regularizers)
    : topic_model_(topic_model),
      token_dict_(token_dict),
      regularizers_(regularizers) {}

void Processor::ItemProcessor::InferTheta(const ModelConfig& model,
                                          const Item& item,
                                          ModelIncrement* model_increment,
                                          float* theta) {
  int topic_size = topic_model_.topic_size();

  if (model_increment != nullptr) {
    model_increment->set_items_processed(model_increment->items_processed() + 1);
  }

  if (model_increment != nullptr) {
    // Process unknown tokens (if any)
    TokenIterator iter(token_dict_, topic_model_, item, model.field_name(),
                        TokenIterator::Mode_Unknown);

    while (iter.Next()) {
      model_increment->add_discovered_token(iter.token());
    }
  }

  // find the id of token in topic_model
  std::vector<int> token_id;
  std::vector<float> token_count;
  std::vector<TopicWeightIterator> token_weights;
  std::vector<float> z_normalizer;
  int known_tokens_count = 0;
  TokenIterator iter(token_dict_, topic_model_, item, model.field_name(),
                      TokenIterator::Mode_Known);

  while (iter.Next()) {
    token_id.push_back(iter.id());
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

        if ((inner_iter == inner_iters_count) && (model_increment != nullptr)) {
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

    //3. The following block of code makes the regularization of theta_next
    for (auto reg_iterator = regularizers_->begin(); reg_iterator != regularizers_->end(); reg_iterator++) {
      std::shared_ptr<int> retval;
      reg_iterator->second->RegularizeTheta(item, theta_next, topic_size, inner_iter, retval);

      if (*retval.get() == REGULARIZATION_FAILED) {
        std::cout << "Problems with type or number of parameters in regularizer " <<
          reg_iterator->first << ". On this iteration this regularizer was turned off.\n";
      }
    }

    // Normalize theta_next. For normal iterations this is handled by curZ value.
    float sum = 0.0f;
    for (int topic_index = 0; topic_index < topic_size; ++topic_index)
      sum += theta_next[topic_index];

    for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
      theta[topic_index] = (sum > 0) ? (theta_next[topic_index] / sum) : 0.0f;
    }
  }
}

void Processor::ItemProcessor::CalculateScore(const Score& score, const Item& item,
                                              const float* theta, double* perplexity,
                                              double* normalizer) {
  int topics_size = topic_model_.topic_size();
  TokenIterator iter(token_dict_, topic_model_, item, score.field_name(),
                      TokenIterator::Mode_Known);
  while (iter.Next()) {
    float sum = 0.0f;
    TopicWeightIterator topic_iter = iter.GetTopicWeightIterator();
    while (topic_iter.NextNonZeroTopic() < topics_size) {
      sum += theta[topic_iter.TopicIndex()] * topic_iter.Weight();
    }

    (*normalizer) += iter.count();
    (*perplexity) += iter.count() * log(sum);
  }
}

Processor::StreamIterator::StreamIterator(const ProcessorInput& processor_input,
                                          const std::string stream_name)
    : items_count_(processor_input.batch().item_size()),
      item_index_(-1),  // // handy trick for iterators
      stream_flags_(nullptr),
      processor_input_(processor_input) {
  int index_of_stream = repeated_field_index_of(processor_input.stream_name(), stream_name);

  if (index_of_stream == -1) {
    // log a warning and process all documents from the stream
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

void Processor::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "Processor thread");
    LOG(INFO) << "Processor thread started";
    for (;;) {
      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      std::shared_ptr<const ProcessorInput> part;
      {
        boost::lock_guard<boost::mutex> guard(*processor_queue_lock_);
        if (processor_queue_->empty()) {
          continue;
        }

        part = processor_queue_->front();
        processor_queue_->pop();
      }

      std::shared_ptr<ProcessorOutput> processor_output = std::make_shared<ProcessorOutput>();
      processor_output->set_batch_uuid(part->batch_uuid());
      processor_output->set_data_loader_id(part->data_loader_id());
      helpers::call_on_destruction c([&]() {
        boost::lock_guard<boost::mutex> guard(*merger_queue_lock_);
        merger_queue_->push(processor_output);
      });

      const ProcessorOutput* previous_processor_output =
        part->has_previous_processor_output() ? &part->previous_processor_output() : nullptr;

      std::shared_ptr<InstanceSchema> schema = schema_.get();
      std::vector<int> model_ids = schema->GetModelIds();
      std::for_each(model_ids.begin(), model_ids.end(), [&](int model_id) {
        const ModelConfig& model = schema->model_config(model_id);

        // do not process disabled models.
        if (!model.enabled()) return;  // return from lambda; goes to next step of std::for_each

        // find cache
        const ModelIncrement* previous_model_increment = nullptr;
        if (previous_processor_output != nullptr) {
          for (int i = 0; i < previous_processor_output->model_increment_size(); ++i) {
            if (previous_processor_output->model_increment(i).model_id() == model_id) {
              previous_model_increment = &previous_processor_output->model_increment(i);
            }
          }
        }

        std::shared_ptr<const TopicModel> topic_model = merger_.GetLatestTopicModel(model_id);
        assert(topic_model.get() != nullptr);

        int topic_size = topic_model->topic_size();
        assert(topic_size > 0);

        // TODO(alfrey): if (cache_old != nullptr), deduct old values

        // process part and store result in merger queue
        auto model_increment = processor_output->add_model_increment();
        model_increment->set_model_id(model_id);
        model_increment->set_items_processed(0);

        // Prepare score vector
        for (int score_index = 0; score_index < model.score_size(); ++score_index) {
          model_increment->add_score(0.0);
          model_increment->add_score_norm(0.0);
        }

        model_increment->set_topics_count(topic_size);

        for (int token_index = 0; token_index < topic_model->token_size(); token_index++) {
          model_increment->add_token(topic_model->token(token_index));
          FloatArray* counters = model_increment->add_token_increment();
          for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
            counters->add_value(0.0f);
          }
        }

        ItemProcessor item_processor(*topic_model, part->batch().token(), 
                                      schema_.get()->GetPointerToRegularizers());
        StreamIterator iter(*part, model.stream_name());
        while (iter.Next() != nullptr) {
          // ToDo: add an option to always start with random iteration!
          const Item* item = iter.Current();

          // ToDo: if (cache_old != nullptr), use it as a starting iteration.
          std::vector<float> theta(topic_size);
          int index_of_item = -1;
          if (previous_model_increment != nullptr) {
            index_of_item = repeated_field_index_of(
              previous_model_increment->item_id(), item->id());
          }

          if ((index_of_item != -1) && model.reuse_theta()) {
            const FloatArray& old_thetas = previous_model_increment->theta(index_of_item);
            for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
              theta[topic_index] = old_thetas.value(topic_index);
            }
          } else {
            for (int iTopic = 0; iTopic < topic_size; ++iTopic) {
              theta[iTopic] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            }
          }

          item_processor.InferTheta(model, *item, model_increment, &theta[0]);

          // Cache theta for the next iteration
          model_increment->add_item_id(item->id());
          FloatArray* cached_theta = model_increment->add_theta();
          for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
            cached_theta->add_value(theta[topic_index]);
          }
        }

        // Calculate all requested scores (such as perplexity)
        for (int score_index = 0; score_index < model.score_size(); ++score_index) {
          const Score& score = model.score(score_index);

          // Perplexity is so far the only score that Processor know how to calculate.
          if (score.type() != Score_Type_Perplexity)
            continue;

          double perplexity_score = 0.0;
          double perplexity_norm = 0.0;
          StreamIterator test_iter(*part, score.stream_name());
          ItemProcessor test_item_processor(*topic_model, part->batch().token(), 
                                              schema_.get()->GetPointerToRegularizers());
          while (test_iter.Next() != nullptr) {
            const Item* item = test_iter.Current();

            std::vector<float> theta_vec;
            theta_vec.resize(topic_size);
            float* theta = &theta_vec[0];
            for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
              theta[topic_index] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            }

            test_item_processor.InferTheta(model, *item, nullptr, theta);
            test_item_processor.CalculateScore(
              score, *item, theta, &perplexity_score, &perplexity_norm);
          }

          model_increment->set_score(score_index,
            model_increment->score(score_index) + perplexity_score);

          model_increment->set_score_norm(
            score_index, model_increment->score_norm(score_index) + perplexity_norm);
        }
      });
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
