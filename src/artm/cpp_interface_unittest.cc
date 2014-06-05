// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/thread.hpp"
#include "gtest/gtest.h"

#include "artm/cpp_interface.h"
#include "artm/messages.pb.h"
#include "artm/internals.pb.h"

TEST(CppInterface, Canary) {
}

void BasicTest(std::string endpoint) {
  const int nTopics = 5;
  bool is_network_mode = !endpoint.empty();

  // Create instance
  artm::MasterComponentConfig master_config;
  if (is_network_mode) master_config.set_master_component_create_endpoint(endpoint);
  artm::MasterComponent master_component(master_config);

  // Create model
  artm::ModelConfig model_config;
  model_config.set_topics_count(nTopics);

  artm::Score* score = model_config.add_score();
  score->set_type(artm::Score_Type_Perplexity);
  artm::Model model(master_component, model_config);

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

  // Index doc-token matrix
  master_component.AddBatch(batch);

  model.Enable();
  std::shared_ptr<artm::TopicModel> topic_model;
  double expected_normalizer = 0;
  for (int iter = 0; iter < 5; ++iter) {
    master_component.InvokeIteration(1);
    master_component.WaitIdle();
    topic_model = master_component.GetTopicModel(model);

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

  master_component.InvokeIteration(3);
  master_component.WaitIdle();
  model.Disable();

  int nUniqueTokens = nTokens;
  EXPECT_EQ(nUniqueTokens, topic_model->token_size());
  auto first_token_topics = topic_model->token_weights(0);
  EXPECT_EQ(first_token_topics.value_size(), nTopics);
}

// To run this particular test:
// artm_tests.exe --gtest_filter=CppInterface.*
TEST(CppInterface, BasicTest_NetworkMode) {
  BasicTest("tcp://localhost:5555");
}

TEST(CppInterface, BasicTest_StandaloneMode) {
  BasicTest("");
}

TEST(CppInterface, Exceptions) {
  // Create instance
  artm::MasterComponentConfig master_config;
  artm::MasterComponent master_component(master_config);

  // Create model
  artm::ModelConfig model_config;
  model_config.set_topics_count(10);
  artm::Model model(master_component, model_config);

  model_config.set_topics_count(20);
  ASSERT_THROW(model.Reconfigure(model_config), artm::UnsupportedReconfiguration);
}
