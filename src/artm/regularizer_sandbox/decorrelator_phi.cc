// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include "artm/regularizer_sandbox/decorrelator_phi.h"
#include "artm/core/regularizable.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace regularizer_sandbox {

bool DecorrelatorPhi::RegularizePhi(::artm::core::Regularizable* topic_model, double tau) {

  const int topic_size = topic_model->topic_size();
  const int background_topics_count = config_.background_topics_count();

  if (background_topics_count < 0 || background_topics_count > topic_size) return false;

  for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {

    auto topic_iterator = topic_model->GetTopicWeightIterator(token_id);
    float weights_sum = 0;
    while (topic_iterator.NextTopic() < topic_size - background_topics_count) {
      weights_sum += topic_iterator.Weight();
    }

    topic_iterator.Reset();
    while (topic_iterator.NextTopic() < topic_size - background_topics_count) {
      float weight = topic_iterator.Weight();
      int topic_id = topic_iterator.TopicIndex();
      float value = static_cast<float>(- tau * weight * (weights_sum - weight));
      topic_model->IncreaseRegularizerWeight(token_id, topic_id, value);
    }
  }
  return true;
}

}  // namespace regularizer_sandbox
}  // namespace artm
