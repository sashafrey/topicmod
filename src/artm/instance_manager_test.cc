#include "gtest/gtest.h"

#include "artm/messages.pb.h"
#include "artm/instance.h"

using namespace ::artm;
using namespace ::artm::core;

// To run this particular test:
// artm_tests.exe --gtest_filter=InstanceManager.*
TEST(InstanceManager, Basic) {
  int id = InstanceManager::singleton().Create(0, InstanceConfig());
  EXPECT_EQ(InstanceManager::singleton().Get(id)->id(), id);

  int id2 = InstanceManager::singleton().Create(0, InstanceConfig());
  EXPECT_EQ(id2, id+1);
  EXPECT_EQ(InstanceManager::singleton().Get(id2)->id(), id2);

  int id3 = InstanceManager::singleton().Create(id2, 
              InstanceConfig());
  EXPECT_EQ(id3, ARTM_ERROR);

  InstanceManager::singleton().Erase(id);
  EXPECT_FALSE(InstanceManager::singleton().Contains(id));

  InstanceManager::singleton().Erase(id2);
}
