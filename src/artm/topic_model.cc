// Copyright 2014, Additive Regularization of Topic Models.

#include <glog/logging.h>

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
    : token_to_token_id_(rhs.token_to_token_id_),
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

TopicModel::~TopicModel() {
  std::for_each(n_wt_.begin(), n_wt_.end(), [&](float* value) {
    delete [] value;
  });

  std::for_each(r_wt_.begin(), r_wt_.end(), [&](float* value) {
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
  n_wt_.push_back(values);
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

  float* regularizer_values = new float[topic_size()];
  for (int i = 0; i < topic_size(); ++i) {
    regularizer_values[i] = 0.0f;
  }
  r_wt_.push_back(regularizer_values);
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
    LOG(ERROR) << "Token '" << token << "' not found in the model";
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
