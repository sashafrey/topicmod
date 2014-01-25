#include "gtest/gtest.h"

#include <boost/thread/mutex.hpp>

#include "topicmd/instance.cc"
#include "topicmd/messages.pb.h"

using namespace topicmd;

TEST(Instance, Basic) {
  Instance instance(1, InstanceConfig());
  Batch batch1;
  batch1.add_token("first token");
  batch1.add_token("second");
  for (int i = 0; i < 2; ++i) {
    Item* item = batch1.add_item();
    Field* field = item->add_field();
    field->add_token_id(i);
    field->add_token_count(i+1);
  }
  instance.InsertBatch(batch1); // +2
  int gen1 = instance.FinishPartition();
  EXPECT_EQ(instance.GetTotalItemsCount(), 0);
  instance.PublishGeneration(gen1);
  EXPECT_EQ(instance.GetTotalItemsCount(), 2);
  
  for (int iBatch = 0; iBatch < 2; ++iBatch) {
    Batch batch;
    for (int i = 0; i < (3 + iBatch); ++i) batch.add_item(); // +3, +4
    instance.InsertBatch(batch);
  }

  int gen2 = instance.FinishPartition();
  instance.PublishGeneration(gen2);

  EXPECT_EQ(instance.GetTotalItemsCount(), 9);

  instance.InsertBatch(batch1);  // +2
  instance.DiscardPartition();   // -2
  instance.InsertBatch(batch1);  // +2
  int gen3 = instance.FinishPartition();
  instance.PublishGeneration(gen3);
  EXPECT_EQ(instance.GetTotalItemsCount(), 11); 

  Batch batch4;
  batch4.add_token("second");
  batch4.add_token("last");
  Item* item = batch4.add_item();
  Field* field = item->add_field();
  for (int iToken = 0; iToken < 2; ++iToken) {
    field->add_token_id(iToken);
    field->add_token_count(iToken + 2);
  }

  instance.InsertBatch(batch4);
  int gen4 = instance.FinishPartition();
  instance.PublishGeneration(gen4);
  
  int model_id = 0;
  ModelConfig config;
  config.set_enabled(true);
  config.set_topics_count(3);
  instance.UpdateModel(model_id, config);

  instance.WaitModelProcessed(model_id, 20);

  config.set_enabled(false);
  instance.UpdateModel(model_id, config);

  ModelTopics model_topics;
  instance.RequestModelTopics(model_id, &model_topics);
  EXPECT_EQ(model_topics.token_topic_size(), 3);
  int found = 0;
  for (int i = 0; i < model_topics.token_topic_size(); ++i) {
    std::string token = model_topics.token_topic(i).token();    
    if (token == "first token") { found++; continue; }
    if (token == "second") { found++; continue; }
    if (token == "last") { found++; continue; }
  }

  EXPECT_EQ(found, 3);
}
