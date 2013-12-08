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
  
  int topics_count = 3;
  Merger merger(instance.get_latest_generation(), topics_count);
  auto ttm = merger.token_topic_matrix();
  EXPECT_EQ(ttm->token_count(), 3);
  EXPECT_TRUE(ttm->token_topics("first token") != NULL);
  EXPECT_TRUE(ttm->token_topics("second") != NULL);
  EXPECT_TRUE(ttm->token_topics("last") != NULL);
  EXPECT_TRUE(ttm->token_topics("not exists") == NULL);

  instance.RunTuningIteration();
}
