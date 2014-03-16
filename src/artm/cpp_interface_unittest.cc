// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/thread.hpp"
#include "gtest/gtest.h"

#include "artm/cpp_interface.h"
#include "artm/messages.pb.h"

TEST(CppInterface, Canary) {
}

// To run this particular test:
// artm_tests.exe --gtest_filter=CppInterface.*
TEST(CppInterface, Basic) {
  const int nTopics = 5;

  // Create instance
  artm::MemcachedServer memcached_server("tcp://*:5555");
  artm::InstanceConfig instance_config;
  instance_config.set_memcached_endpoint("tcp://localhost:5555");
  artm::Instance instance(instance_config);

  // Create model
  artm::ModelConfig model_config;
  model_config.set_topics_count(nTopics);
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
  artm::DataLoader data_loader(instance, config);
  // Index doc-token matrix
  data_loader.AddBatch(batch);

  model.Enable();
  data_loader.InvokeIteration(3);
  data_loader.WaitIdle();
  model.Disable();

  // Request model topics
  std::shared_ptr<artm::ModelTopics> model_topics = instance.GetTopics(model);

  int nUniqueTokens = nTokens;
  EXPECT_EQ(nUniqueTokens, model_topics->token_topic_size());
  const artm::TokenTopics& first_token_topics = model_topics->token_topic(0);
  EXPECT_EQ(first_token_topics.topic_weight_size(), nTopics);
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
