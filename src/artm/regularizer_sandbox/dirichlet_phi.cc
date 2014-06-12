// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/regularizer_sandbox/dirichlet_phi.h"

namespace artm {
namespace regularizer_sandbox {

bool DirichletPhi::RegularizePhi(::artm::core::TopicModel* topic_model, double tau,
  std::vector<std::pair<std::string, 
  std::shared_ptr<std::map<std::string, DictionaryEntry>> >> 
  dictionaries) {

  if (dictionaries.begin() == dictionaries.end()) {
    for (int topic_id = 0; topic_id < topic_model->topic_size(); ++topic_id) {
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        float value = static_cast<float>(tau * 1);
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }
    }
  } else {
    // NOTE! Only the first dictionary will be used as container for parameters

    for (int topic_id = 0; topic_id < topic_model->topic_size(); ++topic_id) {
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        std::string token = topic_model->token(token_id);
        auto dictionary_ptr = dictionaries.begin()->second;
        
        float coef = 0;
        if (dictionary_ptr->find(token) != dictionary_ptr->end()) {
          coef = dictionary_ptr->find(token)->second.value();
        }

        float value = static_cast<float>(tau * coef);
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }
    }
  }
  return true;
}

}  // namespace regularizer_sandbox
}  // namespace artm
