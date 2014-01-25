#include "gtest/gtest.h"

#include <boost/thread.hpp>

#include "topicmd/cpp_interface.h"
#include "topicmd/messages.pb.h"

using namespace topicmd;

TEST(CppInterface, Canary) {
}

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
  int nDocs = 5;

  Batch batch;
  for (int i = 0; i < nTokens; i++) {
    batch.add_token("token");
  }

  batch.add_token("second_token");

  for (int iDoc = 0; iDoc < nDocs; iDoc++) {
    Item* item = batch.add_item();
    Field* field = item->add_field();
    for (int iToken = 0; iToken < nTokens; ++iToken) {
      field->add_token_id(iToken);
      field->add_token_count(iDoc + iToken);
    }
  }

  EXPECT_EQ(batch.item().size(), nDocs);
  for (int i = 0; i < batch.item().size(); i++) {
    EXPECT_EQ(batch.item().Get(i).field().Get(0).token_id().size(), 
        nTokens);
  }

  // Index doc-token matrix
  insert_batch(instance_id, batch);
  int generation_id = finish_partition(instance_id);
  publish_generation(instance_id, generation_id);
  
  model_config.set_enabled(true);
  reconfigure_model(instance_id, model_id, model_config);

  boost::this_thread::sleep(boost::posix_time::milliseconds(50));

  model_config.set_enabled(false);
  reconfigure_model(instance_id, model_id, model_config);

  // Request model topics
  ModelTopics model_topics;
  request_model_topics(instance_id, model_id, &model_topics);

  int nUniqueTokens = 2; // "token" and "second_token"
  EXPECT_EQ(model_topics.token_topic_size(), nUniqueTokens);
  const TokenTopics& first_token_topics = model_topics.token_topic(0);
  EXPECT_EQ(first_token_topics.topic_weight_size(), nTopics);

  dispose_model(instance_id, model_id);

  reconfigure_instance(instance_id, InstanceConfig());

  dispose_instance(instance_id);

  EXPECT_EQ(1, 1);
}
