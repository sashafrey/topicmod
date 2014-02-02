#ifndef TOPICMD_TOKEN_TOPIC_MATRIX
#define TOPICMD_TOKEN_TOPIC_MATRIX

#include <assert.h>

#include <string>

#include <boost/utility.hpp>

namespace topicmd {

class TokenTopicMatrix
{
 private:
  std::map<std::string, int> token_to_token_id_;
  std::vector<std::string> token_id_to_token_;
  int topics_count_;

  // Statistics: how many documents in total 
  // have made a contribution into this token topic matrix
  int items_processed_; 

  std::vector<float*> data_; // vector of length tokens_count
  std::vector<float> normalizer_; // normalization constant for each topic
 public:
  explicit TokenTopicMatrix(int topics_count) :
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

  explicit TokenTopicMatrix(const TokenTopicMatrix& rhs) :
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

  ~TokenTopicMatrix() {
    std::for_each(data_.begin(), data_.end(), [&](float* value) {
      delete [] value; 
    }); 
  }

  int tokens_count() const {
    return data_.size();
  }

  int topics_count() const {
    return topics_count_;
  }

  int items_processed() const {
    return items_processed_;
  }

  void add_items_processed(int value) {
    items_processed_ += value;
  }

  void add_token(const std::string& token) {
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

  int token_id(const std::string& token) const {
    auto iter = token_to_token_id_.find(token);
    if (iter == token_to_token_id_.end()) {
      return -1;
    }

    return iter->second;
  }

  std::string token(int index) const {
    assert(index < tokens_count());
    return token_id_to_token_[index];
  }
  
  const float* token_topics(const std::string& token) const {
    auto iter = token_to_token_id_.find(token);
    if (iter == token_to_token_id_.end()) {
      return NULL;
    }

    return data_[iter->second];
  }

  const float* token_topics(int token_id) const {
    assert(token_id >= 0);
    assert(token_id < tokens_count());
    return data_[token_id];
  }

  const float* normalizer() const {
    return &normalizer_[0];
  }
    
  void add_token_topic(int token_id, int topic_id, float value) {
    data_[token_id][topic_id] += value;
    normalizer_[topic_id] += value;
  }
};

} // namespace topicmd

#endif // TOPICMD_TOKEN_TOPIC_MATRIX
