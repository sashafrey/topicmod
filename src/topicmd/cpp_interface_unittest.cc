#include "gtest/gtest.h"

#include <boost/thread.hpp>

#include "topicmd/cpp_interface.h"
#include "topicmd/messages.pb.h"

using namespace topicmd;

TEST(CppInterface, Canary) {
}

// To run this particular test:
// topicmd_tests.exe --gtest_filter=CppInterface.*
TEST(CppInterface, Basic) {
  const int nTopics = 5;

  // Create instance
  InstanceConfig instance_config;
  int instance_id = create_instance(0, instance_config);

  // Create model
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
  int model_id = create_model(instance_id, 0, model_config);
  
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
  config.set_instance_id(instance_id);
  int data_loader_id = create_data_loader(0, config);
  
  // Index doc-token matrix
  add_batch(data_loader_id, batch);
  
  model_config.set_enabled(true);
  reconfigure_model(instance_id, model_id, model_config);

  boost::this_thread::sleep(boost::posix_time::milliseconds(50));

  model_config.set_enabled(false);
  reconfigure_model(instance_id, model_id, model_config);

  // Request model topics
  ModelTopics model_topics;
  request_model_topics(instance_id, model_id, &model_topics);

  int nUniqueTokens = nTokens;
  EXPECT_EQ(nUniqueTokens, model_topics.token_topic_size());
  const TokenTopics& first_token_topics = model_topics.token_topic(0);
  EXPECT_EQ(first_token_topics.topic_weight_size(), nTopics);

  dispose_model(instance_id, model_id);

  reconfigure_instance(instance_id, InstanceConfig());

  dispose_instance(instance_id);

  dispose_data_loader(data_loader_id);

  EXPECT_EQ(1, 1);
}
