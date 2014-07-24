// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/score_sandbox/top_tokens.h"

#include <utility>
#include <algorithm>

#include "artm/core/exceptions.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace score_sandbox {

void TopTokens::CalculateScore(const artm::core::TopicModel& topic_model, Score* score) {
  int topics_size = topic_model.topic_size();
  int tokens_size = topic_model.token_size();

  if (config_.topic_id_size() == 0) {
    for (int topic_id = 0; topic_id < topics_size; topic_id++) {
      config_.add_topic_id(topic_id);
    }
  }

  std::vector<std::vector<std::pair<float, std::string>>> p_wt;
  for (int topic_index = 0; topic_index < config_.topic_id_size(); topic_index++) {
    p_wt.push_back(std::vector<std::pair<float, std::string>>());
  }

  for (int token_index = 0; token_index < tokens_size; token_index++) {
    std::string token = topic_model.token(token_index);
    ::artm::core::TopicWeightIterator topic_iter = topic_model.GetTopicWeightIterator(token);
    int topic_index = 0;
    while (topic_iter.NextTopic() < topics_size && topic_index < config_.topic_id_size()) {
      if (topic_iter.TopicIndex() < config_.topic_id(topic_index)) {
        continue;
      }
      float weight = topic_iter.Weight();
      p_wt[topic_index].push_back(std::pair<float, std::string>(weight, token));
      ++topic_index;
    }
  }

  TopTokensScore top_tokens_score;

  for (int topic_index = 0; topic_index < config_.topic_id_size(); topic_index++) {
    auto top_tokens = top_tokens_score.add_values();
    std::sort(p_wt[topic_index].begin(), p_wt[topic_index].end());
    for (size_t token_index = p_wt[topic_index].size() - 1;
         (token_index >= 0) && (token_index >= p_wt[topic_index].size() - config_.num_tokens());
         token_index--) {
      top_tokens->add_value(p_wt[topic_index][token_index].second);
    }
  }

  AppendScore(top_tokens_score, score);
}

std::shared_ptr<Score> TopTokens::CreateScore() {
  return std::make_shared<TopTokensScore>();
}

void TopTokens::AppendScore(const Score& score, Score* target) {
  std::string error_message = "Unable downcast Score to SparsityPhiScore";
  const TopTokensScore* top_tokens_score = dynamic_cast<const TopTokensScore*>(&score);
  if (top_tokens_score == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  TopTokensScore* top_tokens_target = dynamic_cast<TopTokensScore*>(target);
  if (top_tokens_target == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  for (int topic_index = 0; topic_index < config_.topic_id_size(); topic_index++) {
    auto top_tokens_item_target = top_tokens_target->add_values();
    auto top_tokens_item_score =  top_tokens_score->values(topic_index);
    for (int token_index = 0; token_index < top_tokens_item_score.value_size(); token_index++) {
      top_tokens_item_target->add_value(top_tokens_item_score.value(token_index));
    }
  }
}

}  // namespace score_sandbox
}  // namespace artm
