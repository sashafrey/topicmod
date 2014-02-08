#ifndef TOPICMD_TOKEN_TOPIC_MATRIX
#define TOPICMD_TOKEN_TOPIC_MATRIX

#include <assert.h>

#include <map>
#include <vector>
#include <string>

#include <boost/utility.hpp>

namespace topicmd {

class TokenWeights
{
  private:
   const float* vector_;
   const float* normalizer_;
   int topics_count_;

  public:
   TokenWeights(const float* vector, const float* normalizer, int topics_count) :
     vector_(vector),
     normalizer_(normalizer),
     topics_count_(topics_count)
   {
     assert(vector != nullptr);
     assert(normalizer != nullptr);
     assert(topics_count > 0);
   }

   inline float at(int topic) const { return vector_[topic] / normalizer_[topic]; }
};

class TokenTopicMatrix
{
 public:
  explicit TokenTopicMatrix(int topics_count);
  explicit TokenTopicMatrix(const TokenTopicMatrix& rhs);
  ~TokenTopicMatrix();

  void AddToken(const std::string& token);
  void IncreaseTokenWeight(int token_id, int topic_id, float value);
  void IncreaseItemsProcessed(int value);

  int tokens_count() const;
  int topics_count() const;
  int items_processed() const;
  int token_id(const std::string& token) const;
  std::string token(int index) const;
  TokenWeights token_weights(const std::string& token) const;
  TokenWeights token_weights(int token_id) const;
private:
  std::map<std::string, int> token_to_token_id_;
  std::vector<std::string> token_id_to_token_;
  int topics_count_;

  // Statistics: how many documents in total 
  // have made a contribution into this token topic matrix
  int items_processed_; 

  std::vector<float*> data_; // vector of length tokens_count
  std::vector<float> normalizer_; // normalization constant for each topic
};

} // namespace topicmd

#endif // TOPICMD_TOKEN_TOPIC_MATRIX
