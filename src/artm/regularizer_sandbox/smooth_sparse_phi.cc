// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/regularizer_sandbox/smooth_sparse_phi.h"

namespace artm {
namespace regularizer_sandbox {

bool SmoothSparsePhi::RegularizePhi(::artm::core::TopicModel* topic_model, double tau) {
  // read the parameters from config
  const int topic_size = topic_model->topic_size();
  const int background_topics_count = config_.background_topics_count();

  const ::google::protobuf::RepeatedPtrField<DoubleArray>& background_beta_vector =
    config_.background_beta();

  // control the correctness of parametres
  if (background_topics_count < 0 || background_topics_count > topic_size) return false;

  for (int i = 0; i < background_beta_vector.size(); ++i) {
    if (background_beta_vector.Get(i).value_size() != topic_model->token_size()) {
      return false;
    }
  }

  // proceed the regularization
  for (int topic_id = 0; topic_id < topic_size; ++topic_id) {
    int usual_topics_count = topic_size - background_topics_count;
    // usual topics
    if (topic_id < usual_topics_count) {
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        if (config_.has_beta()) {
          const DoubleArray beta = config_.beta();
          if (beta.value_size() != topic_model->token_size()) return false;

          float value = static_cast<float>(tau * beta.value(token_id));
          topic_model->SetRegularizerWeight(token_id, topic_id, value);
        } else {
          float value = static_cast<float>(tau * 1);
          topic_model->SetRegularizerWeight(token_id, topic_id, value);
        }
      }
    } else {  // background topics
      if (topic_id + usual_topics_count < background_beta_vector.size()) {
        const DoubleArray temp_beta =
          background_beta_vector.Get(topic_id - usual_topics_count);

        for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
          float value = static_cast<float>(tau * temp_beta.value(token_id));
          topic_model->SetRegularizerWeight(token_id, topic_id, value);
        }
      } else {
        for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
          float value = static_cast<float>(tau * 1);
          topic_model->SetRegularizerWeight(token_id, topic_id, value);
        }
      }
    }
  }
  return true;
}

}  // namespace regularizer_sandbox
}  // namespace artm
