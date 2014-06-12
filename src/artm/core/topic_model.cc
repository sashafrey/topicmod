// Copyright 2014, Additive Regularization of Topic Models.

#include <artm/core/topic_model.h>

#include <assert.h>

#include <algorithm>
#include <string>

#include "glog/logging.h"

#include "artm/core/exceptions.h"

namespace artm {
namespace core {

TopicModel::TopicModel(ModelName model_name, int topics_count, int scores_count)
    : model_name_(model_name),
      token_to_token_id_(),
      token_id_to_token_(),
      topics_count_(topics_count),
      items_processed_(0),
      scores_(),
      scores_norm_(),
      n_wt_(),
      r_wt_(),
      n_t_() {
  assert(topics_count_ > 0);
  n_t_.resize(topics_count_);
  memset(&n_t_[0], 0, sizeof(float) * topics_count_);

  scores_.resize(scores_count);
  scores_norm_.resize(scores_count);
}

TopicModel::TopicModel(const TopicModel& rhs)
    : model_name_(rhs.model_name_),
      token_to_token_id_(rhs.token_to_token_id_),
      token_id_to_token_(rhs.token_id_to_token_),
      topics_count_(rhs.topics_count_),
      items_processed_(rhs.items_processed_),
      scores_(rhs.scores_),
      scores_norm_(rhs.scores_norm_),
      n_wt_(),  // must be deep-copied
      r_wt_(),  // must be deep-copied
      n_t_(rhs.n_t_) {
  for (size_t i = 0; i < rhs.n_wt_.size(); i++) {
    float* values = new float[topics_count_];
    n_wt_.push_back(values);
    memcpy(values, rhs.n_wt_[i], sizeof(float) * topics_count_);
  }

  for (size_t i = 0; i < rhs.r_wt_.size(); i++) {
    float* values = new float[topics_count_];
    r_wt_.push_back(values);
    memcpy(values, rhs.r_wt_[i], sizeof(float) * topics_count_);
  }
}

TopicModel::TopicModel(const ::artm::TopicModel& external_topic_model) {
  CopyFromExternalTopicModel(external_topic_model);
}

TopicModel::TopicModel(const ::artm::core::ModelIncrement& model_increment) {
  model_name_ = model_increment.model_name();
  topics_count_ = model_increment.topics_count();
  items_processed_ = 0;

  scores_.resize(model_increment.score_size());
  scores_norm_.resize(model_increment.score_size());

  n_t_.resize(topics_count_);

  ApplyDiff(model_increment);
}

TopicModel::~TopicModel() {
  Clear(model_name(), topic_size(), score_size());
}

void TopicModel::Clear(ModelName model_name, int topics_count, int scores_count) {
  std::for_each(n_wt_.begin(), n_wt_.end(), [&](float* value) {
    delete [] value;
  });

  std::for_each(r_wt_.begin(), r_wt_.end(), [&](float* value) {
    delete [] value;
  });

  model_name_ = model_name;
  topics_count_ = topics_count;
  items_processed_ = 0;

  token_to_token_id_.clear();
  token_id_to_token_.clear();
  n_wt_.clear();
  r_wt_.clear();

  scores_.clear();
  scores_.resize(scores_count);

  scores_norm_.clear();
  scores_norm_.resize(scores_count);

  n_t_.clear();
  n_t_.resize(topics_count);
}

void TopicModel::CalculateDiff(const ::artm::core::TopicModel& rhs, ::artm::core::ModelIncrement* diff) const {
  if (rhs.topic_size() != topic_size()) {
    std::stringstream message;
    message << "TopicModel::CalculateDiff fails due to mismatch: ";
    message << "rhs.topic_size() == "  << rhs.topic_size() << ", ";
    message << "this->topic_size() == " << topic_size();
    BOOST_THROW_EXCEPTION(std::invalid_argument(message.str()));
  }

  diff->set_model_name(model_name_);
  diff->set_topics_count(topic_size());
  diff->set_items_processed(items_processed() - rhs.items_processed());

  for (int token_index = 0; token_index < token_size(); ++token_index) {
    if (rhs.has_token(token(token_index))) {
      diff->add_token(token(token_index));
      ::artm::FloatArray* token_increment = diff->add_token_increment();
      for (int topic_index = 0; topic_index < topic_size(); ++topic_index) {
        token_increment->add_value(n_wt_[token_index][topic_index] - rhs.n_wt_[token_index][topic_index]);
      }
    } else {
      diff->add_discovered_token(token(token_index));
    }
  }

  if (score_size() == rhs.score_size()) {
    for (int score_index = 0; score_index < score_size(); ++score_index) {
      diff->add_score(scores_[score_index] - rhs.scores_[score_index]);
      diff->add_score_norm(scores_norm_[score_index] - rhs.scores_norm_[score_index]);
    }
  } else {
    LOG(WARNING) << "TopicModel::CalculateDiff failed to diff scores.";
  }
}

void TopicModel::ApplyDiff(const ::artm::core::ModelIncrement& diff) {
  this->IncreaseItemsProcessed(diff.items_processed());
  for (int score_index = 0; score_index < diff.score_size(); ++score_index) {
    this->IncreaseScores(score_index, diff.score(score_index),
                         diff.score_norm(score_index));
  }

  // Add new tokens discovered by processor
  for (int token_index = 0;
       token_index < diff.discovered_token_size();
       ++token_index) {
    std::string new_token = diff.discovered_token(token_index);
    if (!this->has_token(new_token)) {
      this->AddToken(new_token);
    }
  }

  int topics_count = this->topic_size();

  for (int token_index = 0;
       token_index < diff.token_increment_size();
       ++token_index) {
    const FloatArray& counters = diff.token_increment(token_index);
    const std::string& token = diff.token(token_index);
    if (!has_token(token)) {
      this->AddToken(token, false);
    }

    for (int topic_index = 0; topic_index < topics_count; ++topic_index) {
      this->IncreaseTokenWeight(token, topic_index, counters.value(topic_index));
    }
  }
}

void TopicModel::RetrieveExternalTopicModel(::artm::TopicModel* topic_model) const {
  // 1. Fill in non-internal part of ::artm::TopicModel
  topic_model->set_name(model_name_);
  topic_model->set_topics_count(topic_size());
  topic_model->set_items_processed(items_processed());

  for (int token_index = 0; token_index < token_size(); ++token_index) {
    topic_model->add_token(token_id_to_token_[token_index]);
    ::artm::FloatArray* weights = topic_model->add_token_weights();
    TopicWeightIterator iter = GetTopicWeightIterator(token_index);
    while (iter.NextTopic() < topic_size()) {
      weights->add_value(iter.Weight());
    }
  }

  for (int score_index = 0; score_index < score_size(); ++score_index) {
    topic_model->mutable_scores()->add_value(score(score_index));
  }

  // 2. Fill in internal part of ::artm::TopicModel
  ::artm::TopicModel_TopicModelInternals topic_model_internals;
  for (int token_index = 0; token_index < token_size(); ++token_index) {
    ::artm::FloatArray* n_wt = topic_model_internals.add_n_wt();
    ::artm::FloatArray* r_wt = topic_model_internals.add_r_wt();
    for (int topic_index = 0; topic_index < topic_size(); ++topic_index) {
      n_wt->add_value(n_wt_[token_index][topic_index]);
      r_wt->add_value(r_wt_[token_index][topic_index]);
    }
  }

  for (int topic_index = 0; topic_index < topic_size(); ++topic_index) {
    topic_model_internals.mutable_n_t()->add_value(n_t_[topic_index]);
  }

  for (int score_index = 0; score_index < score_size(); ++score_index) {
    topic_model_internals.mutable_scores_raw()->add_value(scores_[score_index]);
    topic_model_internals.mutable_scores_normalizer()->add_value(scores_norm_[score_index]);
  }

  topic_model->set_internals(topic_model_internals.SerializeAsString());
}

void TopicModel::CopyFromExternalTopicModel(const ::artm::TopicModel& external_topic_model) {
  int scores_size = external_topic_model.scores().value_size();
  Clear(external_topic_model.name(), external_topic_model.topics_count(), scores_size);

  items_processed_ = external_topic_model.items_processed();

  ::artm::TopicModel_TopicModelInternals topic_model_internals;
  if (!topic_model_internals.ParseFromString(external_topic_model.internals())) {
    std::stringstream error_message;
    error_message << "Unable to deserialize internals of topic model, model_name="
                  << external_topic_model.name();
    BOOST_THROW_EXCEPTION(SerializationException(error_message.str()));
  }

  for (int token_index = 0; token_index < external_topic_model.token_size(); ++token_index) {
    const std::string& token = external_topic_model.token(token_index);
    auto n_wt = topic_model_internals.n_wt(token_index);
    auto r_wt = topic_model_internals.r_wt(token_index);

    int token_id = AddToken(token, false);
    for (int topic_index = 0; topic_index < topics_count_; ++topic_index) {
      SetTokenWeight(token_id, topic_index, n_wt.value(topic_index));
      SetRegularizerWeight(token_id, topic_index, r_wt.value(topic_index));
    }
  }

  for (int score_index = 0; score_index < scores_size; ++score_index) {
    SetScores(score_index,
              topic_model_internals.scores_raw().value(score_index),
              topic_model_internals.scores_normalizer().value(score_index));
  }
}

int TopicModel::AddToken(const std::string& token, bool random_init) {
  auto iter = token_to_token_id_.find(token);
  if (iter != token_to_token_id_.end()) {
    return iter->second;
  }

  int token_id = token_size();
  token_to_token_id_.insert(
      std::make_pair(token, token_id));
  token_id_to_token_.push_back(token);
  float* values = new float[topic_size()];
  n_wt_.push_back(values);

  if (random_init) {
    float sum = 0.0f;

    for (int i = 0; i < topic_size(); ++i) {
      float val = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      values[i] = val;
      sum += val;
    }

    for (int i = 0; i < topic_size(); ++i) {
      values[i] /= sum;
      n_t_[i] += values[i];
    }
  } else {
    memset(values, 0, sizeof(float) * topic_size());
  }

  float* regularizer_values = new float[topic_size()];
  for (int i = 0; i < topic_size(); ++i) {
    regularizer_values[i] = 0.0f;
  }

  r_wt_.push_back(regularizer_values);

  return token_id;
}

void TopicModel::IncreaseItemsProcessed(int value) {
  items_processed_ += value;
}

void TopicModel::SetItemsProcessed(int value) {
  items_processed_ = value;
}

void TopicModel::IncreaseScores(int score_index, double value, double norm) {
  assert(score_index < static_cast<int>(scores_.size()));
  scores_[score_index] += value;
  scores_norm_[score_index] += norm;
}

void TopicModel::SetScores(int score_index, double value, double norm) {
  assert(score_index < static_cast<int>(scores_.size()));
  scores_[score_index] = value;
  scores_norm_[score_index] = norm;
}

double TopicModel::score(int score_index) const {
  // The only supported type so far is perplexity.
  return exp(- scores_[score_index] / scores_norm_[score_index]);
}

double TopicModel::score_not_normalized(int score_index) const {
  if (score_index >= score_size()) return 0.0f;
  return scores_[score_index];
}

double TopicModel::score_normalizer(int score_index) const {
  if (score_index >= score_size()) return 0.0f;
  return scores_norm_[score_index];
}

void TopicModel::IncreaseTokenWeight(const std::string& token, int topic_id, float value) {
  if (!has_token(token)) {
    if (value != 0.0f) {
      LOG(ERROR) << "Token '" << token << "' not found in the model";
    }

    return;
  }

  IncreaseTokenWeight(token_id(token), topic_id, value);
}

void TopicModel::IncreaseTokenWeight(int token_id, int topic_id, float value) {
  float old_data_value = n_wt_[token_id][topic_id];
  n_wt_[token_id][topic_id] += value;

  if (old_data_value + r_wt_[token_id][topic_id] < 0) {
    if (n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id] > 0) {
      n_t_[topic_id] += n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id];
    }
  } else {
    if (n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id] > 0) {
      n_t_[topic_id] += value;
    } else {
      n_t_[topic_id] -= (old_data_value + r_wt_[token_id][topic_id]);
    }
  }
}

void TopicModel::SetTokenWeight(const std::string& token, int topic_id, float value) {
  if (!has_token(token)) {
    LOG(ERROR) << "Token '" << token << "' not found in the model";
    return;
  }

  SetTokenWeight(token_id(token), topic_id, value);
}

void TopicModel::SetTokenWeight(int token_id, int topic_id, float value) {
  float old_data_value = n_wt_[token_id][topic_id];
  n_wt_[token_id][topic_id] = value;

  if (old_data_value + r_wt_[token_id][topic_id] < 0) {
    if (n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id] > 0) {
      n_t_[topic_id] += n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id];
    }
  } else {
    if (n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id] > 0) {
      n_t_[topic_id] += (n_wt_[token_id][topic_id] - old_data_value);
    } else {
      n_t_[topic_id] -= (old_data_value + r_wt_[token_id][topic_id]);
    }
  }
}

void TopicModel::SetRegularizerWeight(const std::string& token, int topic_id, float value) {
  if (!has_token(token)) {
    LOG(ERROR) << "Token '" << token << "' not found in the model";
    return;
  }

  SetRegularizerWeight(token_id(token), topic_id, value);
}

void TopicModel::SetRegularizerWeight(int token_id, int topic_id, float value) {
  float old_regularizer_value = r_wt_[token_id][topic_id];
  r_wt_[token_id][topic_id] = value;

  if (n_wt_[token_id][topic_id] + old_regularizer_value < 0) {
    if (n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id] > 0) {
      n_t_[topic_id] += n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id];
    }
  } else {
    if (n_wt_[token_id][topic_id] + r_wt_[token_id][topic_id] > 0) {
      n_t_[topic_id] += (r_wt_[token_id][topic_id] - old_regularizer_value);
    } else {
      n_t_[topic_id] -= (n_wt_[token_id][topic_id] + old_regularizer_value);
    }
  }
}

int TopicModel::token_size() const {
  return n_wt_.size();
}

int TopicModel::topic_size() const {
  return topics_count_;
}

ModelName TopicModel::model_name() const {
  return model_name_;
}

int TopicModel::items_processed() const {
  return items_processed_;
}

int TopicModel::score_size() const {
  return scores_.size();
}

bool TopicModel::has_token(const std::string& token) const {
  return token_to_token_id_.find(token) != token_to_token_id_.end();
}

int TopicModel::token_id(const std::string& token) const {
  assert(has_token(token));
  return token_to_token_id_.find(token)->second;
}

std::string TopicModel::token(int index) const {
  assert(index < token_size());
  return token_id_to_token_[index];
}

TopicWeightIterator TopicModel::GetTopicWeightIterator(const std::string& token) const {
  auto iter = token_to_token_id_.find(token);
  return std::move(TopicWeightIterator(n_wt_[iter->second], r_wt_[iter->second],
    &n_t_[0], topics_count_));
}

TopicWeightIterator TopicModel::GetTopicWeightIterator(int token_id) const {
  assert(token_id >= 0);
  assert(token_id < token_size());
  return std::move(TopicWeightIterator(n_wt_[token_id], r_wt_[token_id],
    &n_t_[0], topics_count_));
}

}  // namespace core
}  // namespace artm
