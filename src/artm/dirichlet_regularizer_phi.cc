// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/dirichlet_regularizer_phi.h"

namespace artm {
namespace core {

bool DirichletRegularizerPhi::RegularizePhi(TopicModel* topic_model) {
  const double beta_0 = config_.beta_0();
  const DoubleArray tilde_beta = config_.tilde_beta();

  if (tilde_beta.value_size() != topic_model->token_size()) {
    return false;
  }
    
  for (int topic_id = 0; topic_id < topic_model->topic_size(); ++topic_id) {
    for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
      float value = static_cast<float>(beta_0 * tilde_beta.value(token_id));
      topic_model->SetRegularizerWeight(token_id, topic_id, value);
    }
  }
  return true;
}

}  // namespace core
}  // namespace artm
