#include "gtest/gtest.h"

#include "topicmd/instance_manager.h"
#include "topicmd/messages.pb.h"

using namespace topicmd;

TEST(InstanceManager, Basic) {
  int id = InstanceManager::get().CreateInstance(0, InstanceConfig());
  EXPECT_EQ(InstanceManager::get().instance(id).id(), id);

  int id2 = InstanceManager::get().CreateInstance(0, InstanceConfig());
  EXPECT_EQ(id2, id+1);
  EXPECT_EQ(InstanceManager::get().instance(id2).id(), id2);

  int id3 = InstanceManager::get().CreateInstance(id2, 
						  InstanceConfig());
  EXPECT_EQ(id3, TOPICMD_ERROR);

  InstanceManager::get().erase_instance(id);
  EXPECT_FALSE(InstanceManager::get().has_instance(id));
}
