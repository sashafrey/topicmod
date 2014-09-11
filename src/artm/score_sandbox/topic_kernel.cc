// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/score_sandbox/topic_kernel.h"

#include <math.h>

#include "artm/core/exceptions.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace score_sandbox {

std::shared_ptr<Score> TopicKernel::CalculateScore(const artm::core::TopicModel& topic_model) {
  int topics_size = topic_model.topic_size();
  int tokens_size = topic_model.token_size();

  // parameters preparation
  double probability_mass_threshold = 0.1;

  ::artm::BoolArray topics_to_score;
  bool has_correct_vector = false;
  if (config_.has_topics_to_score()) {
    if (config_.topics_to_score().value_size() != topics_size) {
      LOG(INFO) << "Score Topic Kernel: len(topics_to_score) must be equal to" <<
        "len(topics_size). All topics will be used in scoring.";
    } else {
      has_correct_vector = true;
      topics_to_score.CopyFrom(config_.topics_to_score());
    }
  }

  if (!has_correct_vector) {
    for (int topic_id = 0; topic_id < topics_size; ++topic_id) {
      topics_to_score.add_value(true);
    }
  }

  if (config_.has_probability_mass_threshold()) {
    probability_mass_threshold = config_.probability_mass_threshold();
    if (probability_mass_threshold < 0 || probability_mass_threshold > 1) {
      probability_mass_threshold = 0.1;
      LOG(INFO) << "Score Topic Kernel: probability_mass_threshold should be " <<
        "in [0,1]. Default value 0.1 was set.";
    } else {
      LOG(INFO) << "Score Topic Kernel: probability_mass_threshold wasn't defined. " <<
        "Default value 0.1 was set.";
    }
  }

  // kernel scores calculation
  // the elements, that corresponds non-used topics, will have value (-1)
  ::artm::DoubleArray* kernel_size = new ::artm::DoubleArray();
  ::artm::DoubleArray* kernel_purity = new ::artm::DoubleArray();
  ::artm::DoubleArray* kernel_contrast = new ::artm::DoubleArray();

  for (int topic_index = 0; topic_index < topics_size; ++topic_index) {
    if (topics_to_score.value(topic_index)) {
        kernel_size->add_value(0.0);
        kernel_purity->add_value(0.0);
        kernel_contrast->add_value(0.0);
    } else {
        kernel_size->add_value(-1);
        kernel_purity->add_value(-1);
        kernel_contrast->add_value(-1);
    }
  }
  
  for (int token_index = 0; token_index < tokens_size; token_index++) {
    ::artm::core::TopicWeightIterator topic_iter = topic_model.GetTopicWeightIterator(token_index);
    
    // calculate normalizer
    double normalizer = 0.0;
    while (topic_iter.NextTopic() < topics_size) {
      if (topics_to_score.value(topic_iter.TopicIndex())) {
        normalizer += topic_iter.Weight() * topic_iter.GetNormalizer()[topic_iter.TopicIndex()];
      }
    }
    topic_iter.Reset();
    while (topic_iter.NextTopic() < topics_size) {
      int topic_index = topic_iter.TopicIndex();
      if (topics_to_score.value(topic_index)) {
        double p_tw = topic_iter.Weight() *
          topic_iter.GetNormalizer()[topic_index] / normalizer;
        
        if (p_tw >= probability_mass_threshold) {
          double value = kernel_size->value(topic_index) + 1;
          kernel_size->set_value(topic_index, value);

          value = kernel_purity->value(topic_index) + topic_iter.Weight();
          kernel_purity->set_value(topic_index, value);

          value = kernel_contrast->value(topic_index) + p_tw;
          kernel_contrast->set_value(topic_index, value);
        }
      }
    }
  }

  // contrast = sum(p(t|w)) / kernel_size
  for (int topic_index = 0; topic_index < topics_size; ++topic_index) {
    double value = 0;
    if (kernel_size->value(topic_index) > config_.eps()) {
      value = kernel_contrast->value(topic_index) / kernel_size->value(topic_index);
    }
    kernel_contrast->set_value(topic_index, value);
  }

  // result transferring
  TopicKernelScore* topic_kernel_score = new TopicKernelScore();
  std::shared_ptr<Score> retval(topic_kernel_score);

  topic_kernel_score->set_allocated_kernel_size(kernel_size);
  topic_kernel_score->set_allocated_kernel_purity(kernel_purity);
  topic_kernel_score->set_allocated_kernel_contrast(kernel_contrast);

  return retval;
}

}  // namespace score_sandbox
}  // namespace artm
