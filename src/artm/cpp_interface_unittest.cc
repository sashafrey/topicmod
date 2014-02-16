#include "gtest/gtest.h"

#include <boost/thread.hpp>

#include "artm/cpp_interface.h"
#include "artm/messages.pb.h"

using namespace artm;

TEST(CppInterface, Canary) {
}

// To run this particular test:
// artm_tests.exe --gtest_filter=CppInterface.*
TEST(CppInterface, Basic) {
  const int nTopics = 5;

  // Create instance
  InstanceConfig instance_config;
  Instance instance(instance_config);

  // Create model
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
  Model model(instance, model_config);
  
  // Load doc-token matrix
  int nTokens = 10;
  int nDocs = 15;

  Batch batch;
  for (int i = 0; i < nTokens; i++) {
    std::stringstream str;
    str << "token" << i;
    batch.add_token(str.str());
  }

  for (int iDoc = 0; iDoc < nDocs; iDoc++) {
    Item* item = batch.add_item();
    Field* field = item->add_field();
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

  DataLoaderConfig config;
  DataLoader data_loader(instance, config);
  // Index doc-token matrix
  data_loader.AddBatch(batch);
  data_loader.InvokeIteration(3);
  
  model.Enable();
  data_loader.WaitIdle();
  model.Disable();

  // Request model topics
  std::shared_ptr<ModelTopics> model_topics = instance.GetTopics(model);

  int nUniqueTokens = nTokens;
  EXPECT_EQ(nUniqueTokens, model_topics->token_topic_size());
  const TokenTopics& first_token_topics = model_topics->token_topic(0);
  EXPECT_EQ(first_token_topics.topic_weight_size(), nTopics);
}
