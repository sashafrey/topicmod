#include "gtest/gtest.h"

#include "topicmd/instance.cc"
#include "topicmd/messages.pb.h"
using namespace topicmd;

TEST(Instance, Basic) {
  Instance instance(1, InstanceConfig());
  Batch batch1;
  for (int i = 0; i < 2; ++i) batch1.add_item();
  instance.InsertBatch(batch1);
  int gen1 = instance.FinishPartition();
  EXPECT_EQ(instance.GetTotalItemsCount(), 0);
  instance.PublishGeneration(gen1);
  EXPECT_EQ(instance.GetTotalItemsCount(), 2);
  
  for (int iBatch = 0; iBatch < 2; ++iBatch) {
    Batch batch;
    for (int i = 0; i < (3 + iBatch); ++i) batch.add_item();
    instance.InsertBatch(batch);
  }

  int gen2 = instance.FinishPartition();
  instance.PublishGeneration(gen2);

  EXPECT_EQ(instance.GetTotalItemsCount(), 9);
}
