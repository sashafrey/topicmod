#include "gtest/gtest.h"

#include "topicmd/instance_manager.h"
#include "topicmd/messages.pb.h"

using namespace topicmd;

TEST(InstanceManager, Basic) {
  int id = InstanceManager::singleton().CreateInstance(0, InstanceConfig());
  EXPECT_EQ(InstanceManager::singleton().instance(id)->id(), id);

  int id2 = InstanceManager::singleton().CreateInstance(0, InstanceConfig());
  EXPECT_EQ(id2, id+1);
  EXPECT_EQ(InstanceManager::singleton().instance(id2)->id(), id2);

  int id3 = InstanceManager::singleton().CreateInstance(id2, 
						  InstanceConfig());
  EXPECT_EQ(id3, TOPICMD_ERROR);

  InstanceManager::singleton().erase_instance(id);
  EXPECT_FALSE(InstanceManager::singleton().has_instance(id));
}
