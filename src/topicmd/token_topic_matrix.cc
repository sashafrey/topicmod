#include <topicmd/token_topic_matrix.h>

#include <algorithm>

namespace topicmd {

TokenTopicMatrix::TokenTopicMatrix(int topics_count) :
    token_to_token_id_(),
    token_id_to_token_(),
    topics_count_(topics_count),
    items_processed_(0),
    data_(),
    normalizer_()
{
  assert(topics_count_ > 0);
  normalizer_.resize(topics_count_);
  memset(&normalizer_[0], 0, sizeof(float) * topics_count_);
}

TokenTopicMatrix::TokenTopicMatrix(const TokenTopicMatrix& rhs) :
    token_to_token_id_(rhs.token_to_token_id_),
    token_id_to_token_(rhs.token_id_to_token_),
    topics_count_(rhs.topics_count_),
    items_processed_(rhs.items_processed_),
    data_(), // must be deep-copied 
    normalizer_(rhs.normalizer_)
{
  for (int i = 0; i < rhs.data_.size(); i++) {
    float* values = new float[topics_count_];
    data_.push_back(values);
    memcpy(values, rhs.data_[i], sizeof(float) * topics_count_);
  }
}

TokenTopicMatrix::~TokenTopicMatrix() {
  std::for_each(data_.begin(), data_.end(), [&](float* value) {
    delete [] value; 
  }); 
}

void TokenTopicMatrix::AddToken(const std::string& token) {
  if (token_to_token_id_.find(token) !=
      token_to_token_id_.end())
  {
    return;
  }
    
  token_to_token_id_.insert(
      std::make_pair(token, tokens_count()));
  token_id_to_token_.push_back(token);
  float* values = new float[topics_count()];      
  data_.push_back(values);
  float sum = 0.0f;
  for (int i = 0; i < topics_count(); ++i) {
    float val = (float)rand() / (float)RAND_MAX;
    values[i] = val;
    sum += val;
  }

  for (int i = 0; i < topics_count(); ++i) {
    values[i] /= sum;
    normalizer_[i] += values[i];
  }
}

void TokenTopicMatrix::IncreaseItemsProcessed(int value) {
  items_processed_ += value;
}

void TokenTopicMatrix::IncreaseTokenWeight(int token_id, int topic_id, float value) {
  data_[token_id][topic_id] += value;
  normalizer_[topic_id] += value;
}

int TokenTopicMatrix::tokens_count() const {
  return data_.size();
}

int TokenTopicMatrix::topics_count() const {
  return topics_count_;
}

int TokenTopicMatrix::items_processed() const {
  return items_processed_;
}

int TokenTopicMatrix::token_id(const std::string& token) const {
  auto iter = token_to_token_id_.find(token);
  if (iter == token_to_token_id_.end()) {
    return -1;
  }

  return iter->second;
}

std::string TokenTopicMatrix::token(int index) const {
  assert(index < tokens_count());
  return token_id_to_token_[index];
}
  
TokenWeights TokenTopicMatrix::token_weights(const std::string& token) const {
  auto iter = token_to_token_id_.find(token);
  return TokenWeights(data_[iter->second], &normalizer_[0], topics_count_);
}

TokenWeights TokenTopicMatrix::token_weights(int token_id) const {
  assert(token_id >= 0);
  assert(token_id < tokens_count());
  return TokenWeights(data_[token_id], &normalizer_[0], topics_count_);
}

}