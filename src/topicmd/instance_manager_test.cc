#include "gtest/gtest.h"

#include "topicmd/messages.pb.h"
#include "topicmd/instance.h"

using namespace topicmd;

// To run this particular test:
// topicmd_tests.exe --gtest_filter=InstanceManager.*
TEST(InstanceManager, Basic) {
  int id = InstanceManager::singleton().Create(0, InstanceConfig());
  EXPECT_EQ(InstanceManager::singleton().Get(id)->id(), id);

  int id2 = InstanceManager::singleton().Create(0, InstanceConfig());
  EXPECT_EQ(id2, id+1);
  EXPECT_EQ(InstanceManager::singleton().Get(id2)->id(), id2);

  int id3 = InstanceManager::singleton().Create(id2, 
              InstanceConfig());
  EXPECT_EQ(id3, TOPICMD_ERROR);

  InstanceManager::singleton().Erase(id);
  EXPECT_FALSE(InstanceManager::singleton().Contains(id));

  InstanceManager::singleton().Erase(id2);
}
