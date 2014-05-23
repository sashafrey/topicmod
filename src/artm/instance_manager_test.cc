// Copyright 2014, Additive Regularization of Topic Models.

#include "gtest/gtest.h"

#include "artm/messages.pb.h"
#include "artm/instance.h"

using ::artm::core::InstanceManager;

// To run this particular test:
// artm_tests.exe --gtest_filter=InstanceManager.*
TEST(InstanceManager, Basic) {
  int id = InstanceManager::singleton().Create(::artm::core::InstanceConfig());
  EXPECT_EQ(InstanceManager::singleton().Get(id)->id(), id);

  int id2 = InstanceManager::singleton().Create(::artm::core::InstanceConfig());
  EXPECT_EQ(id2, id+1);
  EXPECT_EQ(InstanceManager::singleton().Get(id2)->id(), id2);

  EXPECT_FALSE(InstanceManager::singleton().TryCreate(id2, ::artm::core::InstanceConfig()));

  InstanceManager::singleton().Erase(id);
  EXPECT_FALSE(InstanceManager::singleton().Get(id) != nullptr);

  InstanceManager::singleton().Erase(id2);
}
