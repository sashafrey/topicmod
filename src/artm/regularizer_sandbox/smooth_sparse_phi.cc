// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>
#include <string>

#include "artm/regularizer_sandbox/smooth_sparse_phi.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace regularizer_sandbox {

bool SmoothSparsePhi::RegularizePhi(::artm::core::TopicModel* topic_model, double tau) {
  // read the parameters from config
  const int topic_size = topic_model->topic_size();
  const int background_topics_count = config_.background_topics_count();

  // control the correctness of parametres
  if (background_topics_count < 0 || background_topics_count > topic_size) return false;

  bool has_dictionary = true;
  if (!config_.has_dictionary_name()) {
    has_dictionary = false;
  }

  auto dictionary_ptr = dictionary(config_.dictionary_name());
  if (has_dictionary && dictionary_ptr == nullptr) {
    has_dictionary = false;
  }


  if (!has_dictionary) {
  // proceed the regularization
    for (int topic_id = 0; topic_id < topic_size; ++topic_id) {
      for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
        float value = static_cast<float>(tau * 1);
        auto topic_iterator = topic_model->GetTopicWeightIterator(token_id);
        value += static_cast<float>((topic_iterator.GetRegularizer())[topic_id]);
        topic_model->SetRegularizerWeight(token_id, topic_id, value);
      }
    }
  } else {
    // proceed the regularization
    for (int topic_id = 0; topic_id < topic_size; ++topic_id) {
      int usual_topics_count = topic_size - background_topics_count;

      // usual topics
      if (topic_id < usual_topics_count) {
        for (int token_id = 0; token_id < topic_model->token_size(); ++token_id) {
          std::string token = topic_model->token(token_id);

          float coef = 0;
          if (dictionary_ptr->find(token) != dictionary_ptr->end()) {
            coef = dictionary_ptr->find(token)->second.value();
          }

          float value = static_cast<float>(tau * coef);
          auto topic_iterator = topic_model->GetTopicWeightIterator(token_id);
          value += static_cast<float>((topic_iterator.GetRegularizer())[topic_id]);
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
          auto topic_iterator = topic_model->GetTopicWeightIterator(token_id);
          value += static_cast<float>((topic_iterator.GetRegularizer())[topic_id]);
          topic_model->SetRegularizerWeight(token_id, topic_id, value);
        }
      }
    }
  }
  return true;
}

}  // namespace regularizer_sandbox
}  // namespace artm
