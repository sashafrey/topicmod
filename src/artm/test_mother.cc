// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/test_mother.h"

namespace artm {
namespace test {

ModelConfig TestMother::GenerateModelConfig() const {
  ModelConfig config;
  config.set_enabled(true);
  config.set_topics_count(nTopics);
  config.add_regularizer_name(regularizer_name);
  ::artm::core::ModelName model_name =
    boost::lexical_cast<std::string>(boost::uuids::random_generator()());
  config.set_name(boost::lexical_cast<std::string>(model_name));
  return config;
}

RegularizerConfig TestMother::GenerateRegularizerConfig() const {
  ::artm::DirichletThetaConfig regularizer_1_config;
  ::artm::DoubleArray tilde_alpha;
  for (int i = 0; i < nTopics; ++i) {
    tilde_alpha.add_value(0);
  }

  for (int i = 0; i < 12; ++i) {
    regularizer_1_config.add_alpha_0(0.01 * (i + 1));
    for (int j = 0; j < nTopics; ++j) {
      tilde_alpha.set_value(j, 0.05 + j * 0.01);
    }
    artm::DoubleArray* tilde_alpha_ptr = regularizer_1_config.add_tilde_alpha();
    *tilde_alpha_ptr = tilde_alpha;
  }

  ::artm::RegularizerConfig general_regularizer_1_config;
  general_regularizer_1_config.set_name(regularizer_name);
  general_regularizer_1_config.set_type(artm::RegularizerConfig_Type_DirichletTheta);
  general_regularizer_1_config.set_config(regularizer_1_config.SerializeAsString());

  return general_regularizer_1_config;
}

}  // namespace test
}  // namespace artm
