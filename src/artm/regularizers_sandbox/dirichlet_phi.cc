// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/regularizers_sandbox/dirichlet_phi.h"

namespace artm {
namespace core {
namespace regularizer {

bool DirichletPhi::RegularizePhi(TopicModel* topic_model, double tau) {
  
  if (config_.has_beta()) {
    const DoubleArray beta = config_.beta();
    if (beta.value_size() != topic_model->token_size()) {
      return false;
    }

    for (int topic_id = 0; topic_id < topic_model->topic_size(); ++topic_id) {
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        float value = static_cast<float>(tau * beta.value(token_id));
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }
    }
  } else {
    for (int topic_id = 0; topic_id < topic_model->topic_size(); ++topic_id) {
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        float value = static_cast<float>(tau * 1);
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }
    }
  }
  return true;
}

}  // namespace regularizer
}  // namespace core
}  // namespace artm
