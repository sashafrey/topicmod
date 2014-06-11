// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/regularizers_sandbox/smooth_sparse_phi.h"

namespace artm {
namespace core {
namespace regularizer {

bool SmoothSparsePhi::RegularizePhi(TopicModel* topic_model, double tau,
  std::vector<std::pair<std::string, 
  std::shared_ptr<std::map<std::string, DictionaryEntry>> >> 
  dictionaries) {

  // read the parameters from config
  const int topic_size = topic_model->topic_size();
  const int background_topics_count = config_.background_topics_count();

  // control the correctness of parametres
  if (background_topics_count < 0 || background_topics_count > topic_size) return false;

  if (dictionaries.begin() == dictionaries.end()) {
  // proceed the regularization
    for (int topic_id = 0; topic_id < topic_size; ++topic_id) {
      int usual_topics_count = topic_size - background_topics_count;
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        float value = static_cast<float>(tau * 1);
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }
    }
  } else {
    // NOTE! Only the first dictionary will be used as container for parameters

    // proceed the regularization
    for (int topic_id = 0; topic_id < topic_size; ++topic_id) {
      int usual_topics_count = topic_size - background_topics_count;
      auto dictionary_ptr = dictionaries.begin()->second;

      // usual topics
      if (topic_id < usual_topics_count) {
        for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
          std::string token = topic_model->token(token_id);
        
          float coef = 0;
          if (dictionary_ptr->find(token) != dictionary_ptr->end()) {
            coef = dictionary_ptr->find(token)->second.value();
          }

          float value = static_cast<float>(tau * coef);
          topic_model->SetRegularizerWeight(token_id, topic_id, value);
        }
      } else {  // background topics
        for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
          std::string token = topic_model->token(token_id);
        
          float coef = 0;
          if (dictionary_ptr->find(token) != dictionary_ptr->end()) {
            int index = topic_id - usual_topics_count;
            coef = dictionary_ptr->find(token)->second.values().value(index);
          }

          float value = static_cast<float>(tau * coef);
          topic_model->SetRegularizerWeight(token_id, topic_id, value);
        }
      }
    }
  }
  return true;
}

}  // namespace regularizer
}  // namespace core
}  // namespace artm
