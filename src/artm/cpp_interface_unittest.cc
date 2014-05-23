// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/thread.hpp"
#include "gtest/gtest.h"

#include "artm/cpp_interface.h"
#include "artm/messages.pb.h"

TEST(CppInterface, Canary) {
}

void BasicTest(std::string endpoint) {
  const int nTopics = 5;
  bool is_network_mode = !endpoint.empty();

  // Create instance
  artm::InstanceConfig instance_config;
  if (is_network_mode) instance_config.set_memcached_endpoint(endpoint);
  artm::Instance instance(instance_config);

  artm::DirichletThetaConfig regularizer_1_config;
  artm::DoubleArray tilde_alpha;
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

  artm::DirichletThetaConfig regularizer_2_config;
  for (int i = 0; i < 10; ++i) {
    regularizer_2_config.add_alpha_0(0.03 * (i + 1));
    for (int j = 0; j < nTopics; ++j) {
      tilde_alpha.set_value(j, 0.08 + j * 0.01);
    }
    artm::DoubleArray* tilde_alpha_ptr = regularizer_2_config.add_tilde_alpha();
    *tilde_alpha_ptr = tilde_alpha;
  }

  std::string regularizer_1_name = "regularizer_1";
  std::string regularizer_2_name = "regularizer_2";

  artm::RegularizerConfig general_regularizer_1_config;
  general_regularizer_1_config.set_name(regularizer_1_name);
  general_regularizer_1_config.set_type(artm::RegularizerConfig_Type_DirichletTheta);
  general_regularizer_1_config.set_config(regularizer_1_config.SerializeAsString());

  artm::RegularizerConfig general_regularizer_2_config;
  general_regularizer_2_config.set_name(regularizer_2_name);
  general_regularizer_2_config.set_type(artm::RegularizerConfig_Type_DirichletTheta);
  general_regularizer_2_config.set_config(regularizer_2_config.SerializeAsString());

  artm::Regularizer regularizer_1(instance, general_regularizer_1_config);
  artm::Regularizer regularizer_2(instance, general_regularizer_2_config);

  // Create model
  artm::ModelConfig model_config;
  model_config.set_topics_count(nTopics);
  model_config.add_regularizer_name(general_regularizer_1_config.name());
  model_config.add_regularizer_name(general_regularizer_2_config.name());

  artm::Score* score = model_config.add_score();
  score->set_type(artm::Score_Type_Perplexity);
  artm::Model model(instance, model_config);

  // Load doc-token matrix
  int nTokens = 10;
  int nDocs = 15;

  artm::Batch batch;
  for (int i = 0; i < nTokens; i++) {
    std::stringstream str;
    str << "token" << i;
    batch.add_token(str.str());
  }

  for (int iDoc = 0; iDoc < nDocs; iDoc++) {
    artm::Item* item = batch.add_item();
    artm::Field* field = item->add_field();
    for (int iToken = 0; iToken < nTokens; ++iToken) {
      field->add_token_id(iToken);
      field->add_token_count(iDoc + iToken + 1);
    }
  }

  EXPECT_EQ(batch.item().size(), nDocs);
  for (int i = 0; i < batch.item().size(); i++) {
    EXPECT_EQ(batch.item().Get(i).field().Get(0).token_id().size(),
        nTokens);
  }

  artm::DataLoaderConfig config;
  if (is_network_mode) config.set_reset_scores(false);
  artm::DataLoader data_loader(instance, config);
  // Index doc-token matrix
  data_loader.AddBatch(batch);

  model.Enable();
  std::shared_ptr<artm::TopicModel> topic_model;
  double expected_normalizer = 0;
  for (int iter = 0; iter < 5; ++iter) {
    data_loader.InvokeIteration(1);
    data_loader.WaitIdle();
    topic_model = instance.GetTopicModel(model);

    ::artm::TopicModel_TopicModelInternals internals;
    internals.ParseFromString(topic_model->internals());
    if (iter == 1) {
      expected_normalizer = internals.scores_normalizer().value(0);
      EXPECT_GT(expected_normalizer, 0);
    } else if (iter >= 2) {
      if (!is_network_mode) {
        // Verify that normalizer does not grow starting from second iteration.
        // This confirms that the Instance::ForceResetScores() function works as expected.
        EXPECT_EQ(internals.scores_normalizer().value(0), expected_normalizer);
      }
    }
  }
  model.Disable();

  int nUniqueTokens = nTokens;
  EXPECT_EQ(nUniqueTokens, topic_model->token_size());
  auto first_token_topics = topic_model->token_weights(0);
  EXPECT_EQ(first_token_topics.value_size(), nTopics);
}

// To run this particular test:
// artm_tests.exe --gtest_filter=CppInterface.*
TEST(CppInterface, BasicTest_NetworkMode) {
  artm::MemcachedServer memcached_server("tcp://*:5555");
  BasicTest("tcp://localhost:5555");
}

TEST(CppInterface, BasicTest_StandaloneMode) {
  BasicTest("");
}

TEST(CppInterface, Exceptions) {
  // Create instance
  artm::InstanceConfig instance_config;
  artm::Instance instance(instance_config);

  // Create model
  artm::ModelConfig model_config;
  model_config.set_topics_count(10);
  artm::Model model(instance, model_config);

  model_config.set_topics_count(20);
  ASSERT_THROW(model.Reconfigure(model_config), artm::UnsupportedReconfiguration);
}
