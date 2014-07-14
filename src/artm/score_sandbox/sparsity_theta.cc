// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/score_sandbox/sparsity_theta.h"

#include <math.h>

#include "artm/core/exceptions.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace score_sandbox {

#define EPS 1e-100

void SparsityTheta::AppendScore(
    const Item& item,
    const google::protobuf::RepeatedPtrField<std::string>& token_dict,
    const artm::core::TopicModel& topic_model,
    const std::vector<float>& theta,
    Score* score) {
  int topics_size = topic_model.topic_size();

  int zero_topics_count = 0;
  for (int topic_index = 0; topic_index < topics_size; ++topic_index) {
    if (abs(theta[topic_index]) < EPS) {
      ++zero_topics_count;
    }
  }

  SparsityThetaScore sparsity_theta_score;
  sparsity_theta_score.set_zero_topics(zero_topics_count);
  sparsity_theta_score.set_total_topics(topics_size);
  AppendScore(sparsity_theta_score, score);
}

std::string SparsityTheta::stream_name() const {
  return config_.stream_name();
}

std::shared_ptr<Score> SparsityTheta::CreateScore() {
  return std::make_shared<SparsityThetaScore>();
}

void SparsityTheta::AppendScore(const Score& score, Score* target) {
  std::string error_message = "Unable downcast Score to SparsityThetaScore";
  const SparsityThetaScore* sparsity_theta_score = dynamic_cast<const SparsityThetaScore*>(&score);
  if (sparsity_theta_score == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  SparsityThetaScore* sparsity_theta_target = dynamic_cast<SparsityThetaScore*>(target);
  if (sparsity_theta_target == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  sparsity_theta_target->set_zero_topics(sparsity_theta_target->zero_topics() +
                                         sparsity_theta_score->zero_topics());
  sparsity_theta_target->set_total_topics(sparsity_theta_target->total_topics() +
                                          sparsity_theta_score->total_topics());
  sparsity_theta_target->set_value((double)sparsity_theta_target->zero_topics() / sparsity_theta_target->total_topics());
}

}  // namespace score_sandbox
}  // namespace artm
