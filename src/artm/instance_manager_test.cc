#include "gtest/gtest.h"

#include "artm/messages.pb.h"
#include "artm/instance.h"

using ::artm::core::InstanceManager;

// To run this particular test:
// artm_tests.exe --gtest_filter=InstanceManager.*
TEST(InstanceManager, Basic) {
  int id = InstanceManager::singleton().Create(0, artm::InstanceConfig());
  EXPECT_EQ(InstanceManager::singleton().Get(id)->id(), id);

  int id2 = InstanceManager::singleton().Create(0, artm::InstanceConfig());
  EXPECT_EQ(id2, id+1);
  EXPECT_EQ(InstanceManager::singleton().Get(id2)->id(), id2);

  int id3 = InstanceManager::singleton().Create(id2, artm::InstanceConfig());
  EXPECT_EQ(id3, ARTM_ERROR);

  InstanceManager::singleton().Erase(id);
  EXPECT_FALSE(InstanceManager::singleton().Get(id) != nullptr);

  InstanceManager::singleton().Erase(id2);
}
