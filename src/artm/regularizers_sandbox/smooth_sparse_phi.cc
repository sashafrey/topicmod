// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/regularizers_sandbox/smooth_sparse_phi.h"

namespace artm {
namespace core {
namespace regularizer {

bool SmoothSparsePhi::RegularizePhi(TopicModel* topic_model) {
  // read the parameters from config
  const int topic_size = topic_model->topic_size();
  const int background_topics_count = config_.background_topics_count();
  const double beta_0 = config_.beta_0();
  const DoubleArray tilde_beta = config_.tilde_beta();
  
  const ::google::protobuf::RepeatedField<double>& background_beta_0_vector = 
    config_.background_beta_0();
  const ::google::protobuf::RepeatedPtrField<DoubleArray>& background_tilde_beta_vector =
    config_.background_tilde_beta();

  // control the correctness of parametres
  if (background_topics_count < 0 || background_topics_count > topic_size) return false;
  if (background_beta_0_vector.size() != background_topics_count) return false;
  if (background_tilde_beta_vector.size() != background_topics_count) return false;
  
  if (tilde_beta.value_size() != topic_model->token_size()) return false;
  for (int i = 0; i < background_topics_count; ++i) {
    if (background_tilde_beta_vector.Get(i).value_size() != topic_model->token_size()) {
      return false;
    }
  }
  
  // proceed the regularization
  for (int topic_id = 0; topic_id < topic_size; ++topic_id) {
    //usual topics
    int usual_topics_count = topic_size - background_topics_count;
    if (topic_id < usual_topics_count) { 
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        float value = static_cast<float>(beta_0 * tilde_beta.value(token_id));
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }
    } else { // background topics
      float temp_beta_0 = background_beta_0_vector.Get(topic_id);
      const DoubleArray temp_tilde_beta = 
        background_tilde_beta_vector.Get(topic_id - usual_topics_count);

      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        float value = static_cast<float>(temp_beta_0 * temp_tilde_beta.value(token_id));
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }      
    }
  }  
  return true;
}

}  // namespace regularizer
}  // namespace core
}  // namespace artm
