#include "gtest/gtest.h"

#include "topicmd/partition.h"
#include "topicmd/messages.pb.h"

using namespace topicmd;

TEST(Partition, Basic) {
  Partition part;
  Batch batch1;
  batch1.add_token("first token");
  batch1.add_token("second");
  for (int i = 0; i < 2; ++i) {
    Item* item = batch1.add_item();
    Field* field = item->add_field();
    field->add_token_id(i);
    field->add_token_count(i+1);
  }

  part.Add(batch1);

  Batch batch4;
  batch4.add_token("second");
  batch4.add_token("last");
  Item* item = batch4.add_item();
  Field* field = item->add_field();
  for (int iToken = 0; iToken < 2; ++iToken) {
    field->add_token_id(iToken);
    field->add_token_count(iToken + 2);
  }

  part.Add(batch4);

  EXPECT_EQ(part.get_item_count(), 3);
  EXPECT_EQ(part.get_tokens().size(), 3);
}
