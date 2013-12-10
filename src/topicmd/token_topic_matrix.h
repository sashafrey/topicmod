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
  float* data_;
 public:
  TokenTopicMatrix() :
      tokens_count_(0),
      topics_count_(0),
      data_(NULL)
  {
  }

  ~TokenTopicMatrix() {
    delete [] data_;
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

  int token_count() const {
    return tokens_count_;
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
  
    
  void Initialize(int topics_count) {
    if (tokens_count_ == 0) return;
      
    assert(data_ == NULL);
    assert(tokens_count_ > 0);
    assert(topics_count > 0);

    topics_count_ = topics_count;
    data_ = new float[ tokens_count_ * topics_count_ ];
    memset(data_, 0, sizeof(float) * tokens_count_ * topics_count_);
  }

  void Initialize(const TokenTopicMatrix& rhs) {
    token_to_token_id_ = rhs.token_to_token_id_;
    token_id_to_token_ = rhs.token_id_to_token_;
    tokens_count_ = rhs.tokens_count_;
    topics_count_ = rhs.topics_count_;
    data_ = new float[ tokens_count_ * topics_count_ ];
    memset(data_, 0, sizeof(float) * tokens_count_ * topics_count_);
  }

  void set_token_topic(int token_id, int topic_id, float value) {
    data_[token_id * topics_count_ + topic_id] = value;
  }

  int tokens_count() const {
    return tokens_count_;
  }

  int topics_count() const {
    return topics_count_;
  }
};

} // namespace topicmd

#endif // TOPICMD_TOKEN_TOPIC_MATRIX
