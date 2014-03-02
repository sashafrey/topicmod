// Copyright 2014, Additive Regularization of Topic Models.

#include <artm/topic_model.h>

#include <assert.h>
#include <string.h>

#include <algorithm>
#include <string>

namespace artm {
namespace core {

TopicModel::TopicModel(int topics_count, int scores_count)
    : token_to_token_id_(),
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
    : token_to_token_id_(rhs.token_to_token_id_),
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

TopicModel::~TopicModel() {
  std::for_each(data_.begin(), data_.end(), [&](float* value) {
    delete [] value;
  });
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

void TopicModel::IncreaseScores(int score_index, double value, double norm) {
  assert(score_index < static_cast<int>(scores_.size()));
  scores_[score_index] += value;
  scores_norm_[score_index] += norm;
}

double TopicModel::score(int iScore) const {
  // The only supported type so far is perplexity.
  return exp(- scores_[iScore] / scores_norm_[iScore]);
}

void TopicModel::IncreaseTokenWeight(const std::string& token, int topic_id, float value) {
  if (!has_token(token)) {
    // TODO(alfrey) Log a warning.
    return;
  }

  IncreaseTokenWeight(token_id(token), topic_id, value);
}

void TopicModel::IncreaseTokenWeight(int token_id, int topic_id, float value) {
  data_[token_id][topic_id] += value;
  normalizer_[topic_id] += value;
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

int TopicModel::has_token(const std::string& token) const {
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