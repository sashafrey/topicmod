#ifndef TOPICMD_TOKEN_TOPIC_MATRIX
#define TOPICMD_TOKEN_TOPIC_MATRIX

#include <assert.h>

#include <string>

#include <boost/utility.hpp>

namespace topicmd {

class TokenTopicMatrix : boost::noncopyable
{
 private:
  std::map<std::string, int> token_to_token_id_;
  std::vector<std::string> token_id_to_token_;
  int tokens_count_;
  int topics_count_;

  // Statistics: how many documents in total 
  // have made a contribution into this token topic matrix
  int items_processed_; 

  float* data_;  
 public:
  TokenTopicMatrix() :
      token_to_token_id_(),
      token_id_to_token_(),
      tokens_count_(0),
      topics_count_(0),
      items_processed_(0),
      data_(NULL)
  {
  }

  ~TokenTopicMatrix() {
    delete [] data_;
  }

  int tokens_count() const {
    return tokens_count_;
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
    if (token_to_token_id_.find(token) ==
        token_to_token_id_.end())
    {
      token_to_token_id_.insert(
          std::make_pair(token, tokens_count_++));
      token_id_to_token_.push_back(token);
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
    assert(index < tokens_count_);
    return token_id_to_token_[index];
  }
  
  float* token_topics(const std::string& token) const {
    auto iter = token_to_token_id_.find(token);
    if (iter == token_to_token_id_.end()) {
      return NULL;
    }

    return &data_[topics_count_ * iter->second];
  }

  float* token_topics(int token_id) const {
    assert(token_id >= 0);
    assert(token_id < tokens_count_);
    return &data_[topics_count_ * token_id];
  }
    
  void Initialize(int topics_count, bool as_random = false) {
    assert(data_ == NULL);
    assert(tokens_count_ >= 0);
    assert(topics_count > 0);

    topics_count_ = topics_count;

    // The following allocation should work even when tokens_count_ == 0
    // http://stackoverflow.com/questions/1087042/c-new-int0-will-it-allocate-memory
    data_ = new float[ tokens_count_ * topics_count_]; 

    if (as_random) {
      for (int i = 0; i < tokens_count_ * topics_count_; ++i) {
        data_[i] = (float)rand() / (float)RAND_MAX;
      }
    } else {
      memset(data_, 0, sizeof(float) * tokens_count_ * topics_count_);
    }
  }

  void Initialize(const TokenTopicMatrix& rhs) {
    token_to_token_id_ = rhs.token_to_token_id_;
    token_id_to_token_ = rhs.token_id_to_token_;
    tokens_count_ = rhs.tokens_count_;
    topics_count_ = rhs.topics_count_;
    items_processed_ = rhs.items_processed_;
    data_ = new float[ tokens_count_ * topics_count_ ];
    memcpy(data_, rhs.data_, sizeof(float) * tokens_count_ * topics_count_);
  }

  void set_token_topic(int token_id, int topic_id, float value) {
    data_[token_id * topics_count_ + topic_id] = value;
  }
};

} // namespace topicmd

#endif // TOPICMD_TOKEN_TOPIC_MATRIX
