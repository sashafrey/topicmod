// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_SCORE_CALCULATOR_INTERFACE_H_
#define SRC_ARTM_SCORE_CALCULATOR_INTERFACE_H_

#include <memory>
#include <string>
#include <vector>

#include "artm/messages.pb.h"

namespace artm {

typedef ::google::protobuf::Message Score;

namespace core {
  class TopicModel;
}

class ScoreCalculatorInterface {
 public:
  ScoreCalculatorInterface() {}
  virtual ~ScoreCalculatorInterface() { }

  virtual ScoreData_Type score_type() const = 0;

  // Non-cumulative calculation (based on Phi matrix)
  virtual void CalculateScore(const artm::core::TopicModel& topic_model, Score* score) {}

  // Cumulative calculation (such as perplexity, or sparsity of Theta matrix)
  virtual bool is_cumulative() const { return false; }
  virtual std::string stream_name() const { return std::string(); }

  virtual std::shared_ptr<Score> CreateScore() { return nullptr; }

  virtual void AppendScore(const Score& score, Score* target) { return; }

  virtual void AppendScore(
      const Item& item,
      const google::protobuf::RepeatedPtrField<std::string>& token_dict,
      const artm::core::TopicModel& topic_model,
      const std::vector<float>& theta,
      Score* score) { }
};

}  // namespace artm

#endif  // SRC_ARTM_SCORE_CALCULATOR_INTERFACE_H_
