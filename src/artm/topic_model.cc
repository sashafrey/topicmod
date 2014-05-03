// Copyright 2014, Additive Regularization of Topic Models.

#include <artm/topic_model.h>

#include <assert.h>

#include <algorithm>
#include <string>

#include "glog/logging.h"

#include "artm/exceptions.h"

namespace artm {
namespace core {

TopicModel::TopicModel(ModelId model_id, int topics_count, int scores_count)
    : model_id_(model_id),
      token_to_token_id_(),
      token_id_to_token_(),
      topics_count_(topics_count),
      items_processed_(0),
      scores_(),
      scores_norm_(),
      data_(),
      normalizer_() {
  assert(topics_count_ > 0);
  normalizer_.resize(topics_count_);
  memset(&normalizer_[0], 0, sizeof(float) * topics_count_);

  scores_.resize(scores_count);
  scores_norm_.resize(scores_count);
}

TopicModel::TopicModel(const TopicModel& rhs)
    : model_id_(rhs.model_id_),
      token_to_token_id_(rhs.token_to_token_id_),
      token_id_to_token_(rhs.token_id_to_token_),
      topics_count_(rhs.topics_count_),
      items_processed_(rhs.items_processed_),
      scores_(rhs.scores_),
      scores_norm_(rhs.scores_norm_),
      data_(),  // must be deep-copied
      normalizer_(rhs.normalizer_) {
  for (size_t i = 0; i < rhs.data_.size(); i++) {
    float* values = new float[topics_count_];
    data_.push_back(values);
    memcpy(values, rhs.data_[i], sizeof(float) * topics_count_);
  }
}

TopicModel::TopicModel(const ::artm::TopicModel& external_topic_model)
    : model_id_(external_topic_model.model_id()),
      token_to_token_id_(),  // require special handling
      token_id_to_token_(),  // require special handling
      topics_count_(external_topic_model.topics_count()),
      items_processed_(external_topic_model.items_processed()),
      scores_(),       // require special handling
      scores_norm_(),  // require special handling
      data_(),         // require special handling
      normalizer_() {  // require special handling
  ::artm::TopicModel_TopicModelInternals topic_model_internals;
  if (!topic_model_internals.ParseFromString(external_topic_model.internals())) {
    std::stringstream error_message;
    error_message << "Unable to deserialize internals of topic model, model_id="
                  << external_topic_model.model_id();
    BOOST_THROW_EXCEPTION(SerializationError(error_message.str()));
  }

  for (int token_index = 0; token_index < external_topic_model.token_size(); ++token_index) {
    const std::string& token = external_topic_model.token(token_index);
    token_id_to_token_.push_back(token);
    token_to_token_id_.insert(std::make_pair(token, token_index));

    float* values = new float[topics_count_];
    data_.push_back(values);

    auto n_wt = topic_model_internals.n_wt(token_index);
    for (int topic_index = 0; topic_index < topics_count_; ++topic_index) {
      values[topic_index] = n_wt.value(topic_index);
    }
  }

  for (int topic_index = 0; topic_index < topics_count_; ++topic_index) {
    normalizer_.push_back(topic_model_internals.n_t().value(topic_index));
  }

  int scores_size = topic_model_internals.scores_normalizer().value_size();
  for (int score_index = 0; score_index < scores_size; ++score_index) {
    scores_.push_back(topic_model_internals.scores_raw().value(score_index));
    scores_norm_.push_back(topic_model_internals.scores_normalizer().value(score_index));
  }
}

TopicModel::~TopicModel() {
  std::for_each(data_.begin(), data_.end(), [&](float* value) {
    delete [] value;
  });
}

void TopicModel::RetrieveExternalTopicModel(::artm::TopicModel* topic_model) const {
  // 1. Fill in non-internal part of ::artm::TopicModel
  topic_model->set_model_id(model_id_);
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
    for (int topic_index = 0; topic_index < topic_size(); ++topic_index) {
      n_wt->add_value(data_[token_index][topic_index]);
    }
  }

  for (int topic_index = 0; topic_index < topic_size(); ++topic_index) {
    topic_model_internals.mutable_n_t()->add_value(normalizer_[topic_index]);
  }

  for (int score_index = 0; score_index < score_size(); ++score_index) {
    topic_model_internals.mutable_scores_raw()->add_value(scores_[score_index]);
    topic_model_internals.mutable_scores_normalizer()->add_value(scores_norm_[score_index]);
  }

  topic_model->set_internals(topic_model_internals.SerializeAsString());
}

void TopicModel::AddToken(const std::string& token) {
  if (token_to_token_id_.find(token) != token_to_token_id_.end()) {
    return;
  }

  token_to_token_id_.insert(
      std::make_pair(token, token_size()));
  token_id_to_token_.push_back(token);
  float* values = new float[topic_size()];
  data_.push_back(values);
  float sum = 0.0f;
  for (int i = 0; i < topic_size(); ++i) {
    float val = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    values[i] = val;
    sum += val;
  }

  for (int i = 0; i < topic_size(); ++i) {
    values[i] /= sum;
    normalizer_[i] += values[i];
  }
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
  data_[token_id][topic_id] += value;
  normalizer_[topic_id] += value;
}

void TopicModel::SetTokenWeight(const std::string& token, int topic_id, float value) {
  if (!has_token(token)) {
    LOG(ERROR) << "Token '" << token << "' not found in the model";
    return;
  }

  SetTokenWeight(token_id(token), topic_id, value);
}

void TopicModel::SetTokenWeight(int token_id, int topic_id, float value) {
  // Adjust normalizer. (!) Don't switch these lines (1) and (2).
  normalizer_[topic_id] += (value - data_[token_id][topic_id]);  // (1)
  data_[token_id][topic_id] = value;  // (2)
}

int TopicModel::token_size() const {
  return data_.size();
}

int TopicModel::topic_size() const {
  return topics_count_;
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
  return std::move(TopicWeightIterator(data_[iter->second], &normalizer_[0], topics_count_));
}

TopicWeightIterator TopicModel::GetTopicWeightIterator(int token_id) const {
  assert(token_id >= 0);
  assert(token_id < token_size());
  return std::move(TopicWeightIterator(data_[token_id], &normalizer_[0], topics_count_));
}

}  // namespace core
}  // namespace artm
