#include "gtest/gtest.h"

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
  int generation_id = finish_generation(instance_id);
  publish_generation(instance_id, generation_id);
  
  // Run one tuning iteration
  run_tuning_iteration(instance_id);

  // Request model topics
  ModelTopics model_topics;
  request_model_topics(instance_id, model_id, &model_topics);

  EXPECT_EQ(model_topics.token_topic_size(), 123);

  dispose_model(instance_id, model_id);
  dispose_instance(instance_id);

  EXPECT_EQ(1, 1);
}
