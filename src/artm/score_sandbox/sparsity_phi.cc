// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/score_sandbox/sparsity_phi.h"

#include <math.h>

#include "artm/core/exceptions.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace score_sandbox {

void SparsityPhi::CalculateScore(const artm::core::TopicModel& topic_model, Score* score) {
  int topics_size = topic_model.topic_size();
  int tokens_size = topic_model.token_size();

  int zero_tokens_count = 0;

  for (int token_index = 0; token_index < tokens_size; token_index++) {
    ::artm::core::TopicWeightIterator topic_iter = topic_model.GetTopicWeightIterator(token_index);
    while (topic_iter.NextTopic() < topics_size) {
      if (abs(topic_iter.Weight()) < config_.eps() ||
          abs(topic_iter.NotNormalizedWeight()) < config_.eps()) {
        ++zero_tokens_count;
      }
    }
  }

  SparsityPhiScore sparsity_phi_score;
  sparsity_phi_score.set_zero_tokens(zero_tokens_count);
  sparsity_phi_score.set_total_tokens(tokens_size * topics_size);
  AppendScore(sparsity_phi_score, score);
}

std::shared_ptr<Score> SparsityPhi::CreateScore() {
  return std::make_shared<SparsityPhiScore>();
}

void SparsityPhi::AppendScore(const Score& score, Score* target) {
  std::string error_message = "Unable downcast Score to SparsityPhiScore";
  const SparsityPhiScore* sparsity_phi_score = dynamic_cast<const SparsityPhiScore*>(&score);
  if (sparsity_phi_score == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  SparsityPhiScore* sparsity_phi_target = dynamic_cast<SparsityPhiScore*>(target);
  if (sparsity_phi_target == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  sparsity_phi_target->set_zero_tokens(sparsity_phi_target->zero_tokens() +
                                       sparsity_phi_score->zero_tokens());
  sparsity_phi_target->set_total_tokens(sparsity_phi_target->total_tokens() +
                                        sparsity_phi_score->total_tokens());
  sparsity_phi_target->set_value(static_cast<double>(sparsity_phi_target->zero_tokens()) /
                                 sparsity_phi_target->total_tokens());
}

}  // namespace score_sandbox
}  // namespace artm
