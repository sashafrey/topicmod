// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_TOPIC_MODEL_H_
#define SRC_ARTM_TOPIC_MODEL_H_

#include <assert.h>

#include <map>
#include <vector>
#include <string>

#include "boost/utility.hpp"

#include "artm/common.h"

namespace artm {
namespace core {

// A class representing an iterator over one row from Phi matrix
// (a vector of topic weights for a particular token of the topic model).
// Remark: currently this method iterates over a dense array, but all methods of this iterator can
// be rewritten to handle sparse token-topic matrices.
// Typical usage is as follows:
// ===============================================================
// TopicWeightIterator iter = topic_model->GetTopicWeightIterator(token);
// while (iter.NextTopic() < topic_model->topic_size()) {
//   values[iter.TopicIndex()] = iter.Weight();
// }
// ===============================================================
class TopicWeightIterator {
 public:
  // Moves the iterator to the next non-zero topic, and return the index of that topic.
  inline int NextNonZeroTopic() { return ++current_topic_; }

  // Moves the iterator to the next topic topic.
  inline int NextTopic() { return ++current_topic_; }

  // Returns current position of the iterator.
  inline int TopicIndex() {return current_topic_; }

  // Returns the weight of current topic.
  // This method must not be called if Current() returns an index exceeding the number of topics.
  // It is caller responsibility to verify this condition.
  inline float Weight() {
    assert(current_topic_ < topics_count_);
    return vector_[current_topic_] / normalizer_[current_topic_];
  }

  // Resets the iterator to the initial state.
  inline void Reset() { current_topic_ = -1; }

 private:
  const float* vector_;
  const float* normalizer_;
  int topics_count_;
  mutable int current_topic_;

  TopicWeightIterator(const float* vector, const float* normalizer, int topics_count)
      : vector_(vector), normalizer_(normalizer), topics_count_(topics_count), current_topic_(-1) {
    assert(vector != nullptr);
    assert(normalizer != nullptr);
  }

  friend class TopicModel;
};

// A class representing a topic model.
// ::artm::core::TopicModel is an internal representation, used between Processor and Merger.
// ::artm::ModelTopics is an external representation, implemented as protobuf message.
// ::artm::ModelTopics should be perhaps renamed to ::artm::TopicModel.
class TopicModel {
 public:
  explicit TopicModel(int topics_count, int scores_count);
  explicit TopicModel(const TopicModel& rhs);
  ~TopicModel();

  void AddToken(const std::string& token);
  void IncreaseTokenWeight(const std::string& token, int topic_id, float value);
  void IncreaseTokenWeight(int token_id, int topic_id, float value);
  void IncreaseItemsProcessed(int value);
  void IncreaseScores(int iScore, double value, double score_norm);
  TopicWeightIterator GetTopicWeightIterator(const std::string& token) const;
  TopicWeightIterator GetTopicWeightIterator(int token_id) const;

  int items_processed() const;
  int score_size() const;
  int token_size() const;
  int topic_size() const;

  double score(int score_index) const;
  int has_token(const std::string& token) const;
  int token_id(const std::string& token) const;
  std::string token(int index) const;

 private:
  std::map<std::string, int> token_to_token_id_;
  std::vector<std::string> token_id_to_token_;
  int topics_count_;

  // Statistics: how many documents in total
  // have made a contribution into this topic model.
  int items_processed_;

  // Scores (such as perplexity), defined by ModelConfig.
  std::vector<double> scores_;
  std::vector<double> scores_norm_;

  std::vector<float*> data_;  // vector of length tokens_count
  std::vector<float> normalizer_;  // normalization constant for each topic
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_TOPIC_MODEL_H_
