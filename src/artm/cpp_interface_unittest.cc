// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/thread.hpp"
#include "gtest/gtest.h"

#include "boost/filesystem.hpp"

#include "artm/cpp_interface.h"
#include "artm/messages.pb.h"
#include "artm/internals.pb.h"

TEST(CppInterface, Canary) {
}

void BasicTest(bool is_network_mode) {
  const int nTopics = 5;

  ::artm::MasterComponentConfig master_config;
  if (is_network_mode) {
    master_config.set_master_component_create_endpoint("tcp://*:5555");
    master_config.set_master_component_connect_endpoint("tcp://localhost:5555");
    master_config.set_disk_path(".");

    // Clean all .batches files
    boost::filesystem::recursive_directory_iterator it(".");
    boost::filesystem::recursive_directory_iterator endit;
    while (it != endit) {
      if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ".batch") {
        boost::filesystem::remove(*it);
      }

      ++it;
    }

    master_config.set_modus_operandi(::artm::MasterComponentConfig_ModusOperandi_Network);
  } else {
    master_config.set_modus_operandi(::artm::MasterComponentConfig_ModusOperandi_Local);
    master_config.set_cache_processor_output(true);
  }

  // Create master component
  artm::MasterComponent master_component(master_config);

  std::shared_ptr<::artm::NodeController> node_controller;
  if (is_network_mode) {
    ::artm::NodeControllerConfig node_config;
    node_config.set_master_component_connect_endpoint("tcp://localhost:5555");
    node_config.set_node_controller_create_endpoint("tcp://*:5556");
    node_config.set_node_controller_connect_endpoint("tcp://localhost:5556");
    node_controller.reset(new ::artm::NodeController(node_config));
    master_component.Reconfigure(master_config);  // Push configuration to client
  }

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

  master_component.InvokeIteration(3);
  master_component.WaitIdle();
  model.Disable();

  int nUniqueTokens = nTokens;
  EXPECT_EQ(nUniqueTokens, topic_model->token_size());
  auto first_token_topics = topic_model->token_weights(0);
  EXPECT_EQ(first_token_topics.value_size(), nTopics);

  if (!is_network_mode) {
    std::shared_ptr<::artm::ThetaMatrix> theta_matrix = master_component.GetThetaMatrix(model);
    EXPECT_TRUE(theta_matrix->item_id_size() == nDocs);
    for (int item_index = 0; item_index < theta_matrix->item_id_size(); ++item_index) {
      const ::artm::FloatArray& weights = theta_matrix->item_weights(item_index);
      EXPECT_EQ(weights.value_size(), nTopics);
      float sum = 0;
      for (int topic_index = 0; topic_index < weights.value_size(); ++topic_index) {
        float weight = weights.value(topic_index);
        EXPECT_GT(weight, 0);
        sum += weight;
      }

      EXPECT_LE(abs(sum - 1), 0.001);
    }
  }
}

// To run this particular test:
// artm_tests.exe --gtest_filter=CppInterface.BasicTest_NetworkMode
TEST(CppInterface, BasicTest_NetworkMode) {
  BasicTest(true);
}

// artm_tests.exe --gtest_filter=CppInterface.BasicTest_StandaloneMode
TEST(CppInterface, BasicTest_StandaloneMode) {
  BasicTest(false);
}

// artm_tests.exe --gtest_filter=CppInterface.Exceptions
TEST(CppInterface, Exceptions) {
  // Create instance
  artm::MasterComponentConfig master_config;
  artm::MasterComponent master_component(master_config);

  // Create model
  artm::ModelConfig model_config;
  model_config.set_topics_count(10);
  artm::Model model(master_component, model_config);

  model.mutable_config()->set_topics_count(20);
  ASSERT_THROW(model.Reconfigure(model.config()), artm::UnsupportedReconfiguration);
}
