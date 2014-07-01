// Copyright 2014, Additive Regularization of Topic Models.

#include "gtest/gtest.h"

#include "artm/messages.pb.h"
#include "artm/core/master_component.h"

using ::artm::core::MasterComponentManager;

// To run this particular test:
// artm_tests.exe --gtest_filter=TemplateManager.*
TEST(TemplateManager, Basic) {
  int id = MasterComponentManager::singleton().Create(::artm::MasterComponentConfig());
  EXPECT_EQ(MasterComponentManager::singleton().Get(id)->id(), id);

  int id2 = MasterComponentManager::singleton().Create(::artm::MasterComponentConfig());
  EXPECT_EQ(id2, id+1);
  EXPECT_EQ(MasterComponentManager::singleton().Get(id2)->id(), id2);

  EXPECT_FALSE(MasterComponentManager::singleton().TryCreate(id2, ::artm::MasterComponentConfig()));

  MasterComponentManager::singleton().Erase(id);
  EXPECT_FALSE(MasterComponentManager::singleton().Get(id) != nullptr);

  MasterComponentManager::singleton().Erase(id2);
}
