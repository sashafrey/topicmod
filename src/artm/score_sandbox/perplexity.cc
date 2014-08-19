// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/score_sandbox/perplexity.h"

#include <math.h>

#include "artm/core/exceptions.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace score_sandbox {

void Perplexity::AppendScore(
      const Item& item,
      const std::vector<artm::core::Token> token_dict,
      const artm::core::TopicModel& topic_model,
      const std::vector<float>& theta,
      Score* score) {
  int topics_size = topic_model.topic_size();

  const Field* field = nullptr;
  for (int field_index = 0; field_index < item.field_size(); field_index++) {
    if (item.field(field_index).name() == config_.field_name()) {
      field = &item.field(field_index);
    }
  }

  if (field == nullptr) {
    LOG(ERROR) << "Unable to find field " << config_.field_name() << " in item " << item.id();
    return;
  }

  int n_d_integer = 0;
  for (int token_index = 0; token_index < field->token_count_size(); ++token_index)
    n_d_integer += field->token_count(token_index);
  float n_d = static_cast<float>(n_d_integer);

  int zero_words = 0;
  double normalizer = 0;
  double raw = 0;

  for (int token_index = 0; token_index < field->token_count_size(); ++token_index) {
    double sum = 0.0;
    const artm::core::Token& token = *(token_dict.begin() + token_index);
    double token_count = static_cast<double>(field->token_count(token_index));

    if (topic_model.has_token(token)) {
      ::artm::core::TopicWeightIterator topic_iter = topic_model.GetTopicWeightIterator(token);
      while (topic_iter.NextNonZeroTopic() < topics_size) {
        sum += theta[topic_iter.TopicIndex()] * topic_iter.Weight();
      }
    }

    if (sum == 0.0) {
      // Use document unigram model
      sum = token_count / n_d;
      zero_words++;
    }

    normalizer += token_count;
    raw        += token_count * log(sum);
  }

  PerplexityScore perplexity_score;
  perplexity_score.set_normalizer(normalizer);
  perplexity_score.set_raw(raw);
  perplexity_score.set_zero_words(zero_words);
  AppendScore(perplexity_score, score);
}

std::string Perplexity::stream_name() const {
  return config_.stream_name();
}

std::shared_ptr<Score> Perplexity::CreateScore() {
  return std::make_shared<PerplexityScore>();
}

void Perplexity::AppendScore(const Score& score, Score* target) {
  std::string error_message = "Unable downcast Score to PerplexityScore";
  const PerplexityScore* perplexity_score = dynamic_cast<const PerplexityScore*>(&score);
  if (perplexity_score == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  PerplexityScore* perplexity_target = dynamic_cast<PerplexityScore*>(target);
  if (perplexity_target == nullptr) {
    BOOST_THROW_EXCEPTION(::artm::core::InvalidOperation(error_message));
  }

  perplexity_target->set_normalizer(perplexity_target->normalizer() +
                                    perplexity_score->normalizer());
  perplexity_target->set_raw(perplexity_target->raw() +
                             perplexity_score->raw());
  perplexity_target->set_zero_words(perplexity_target->zero_words() +
                                    perplexity_score->zero_words());
  perplexity_target->set_value(exp(- perplexity_target->raw() / perplexity_target->normalizer()));
}

}  // namespace score_sandbox
}  // namespace artm
