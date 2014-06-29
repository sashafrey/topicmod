// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/thread.hpp"

#include "gtest/gtest.h"

#include "artm/core/node_controller.h"
#include "artm/core/master_component.h"
#include "artm/messages.pb.h"
#include "artm/core/instance.h"
#include "artm/core/data_loader.h"
#include "artm_tests/test_mother.h"
#include "artm/core/instance_schema.h"

// artm_tests.exe --gtest_filter=NodesConnectivityTest.*
TEST(NodesConnectivityTest, Basic) {
  ::artm::test::TestMother test_mother;
  ::artm::MasterComponentConfig master_config;
  master_config.set_modus_operandi(::artm::MasterComponentConfig_ModusOperandi_Network);
  master_config.set_master_component_create_endpoint("tcp://*:5555");
  master_config.set_master_component_connect_endpoint("tcp://localhost:5555");
  master_config.set_disk_path(".");
  int master_id = artm::core::MasterComponentManager::singleton().Create(master_config);
  auto master = artm::core::MasterComponentManager::singleton().Get(master_id);
  ASSERT_EQ(master->clients_size(), 0);

  ::artm::NodeControllerConfig node_config;
  node_config.set_master_component_connect_endpoint("tcp://localhost:5555");
  node_config.set_node_controller_create_endpoint("tcp://*:5556");
  node_config.set_node_controller_connect_endpoint("tcp://localhost:5556");
  int node_id = artm::core::NodeControllerManager::singleton().Create(node_config);
  EXPECT_EQ(master->clients_size(), 1);

  EXPECT_TRUE(::artm::core::InstanceManager::singleton().First() == nullptr);

  // Push configuration to all clients
  master->Reconfigure(master_config);

  EXPECT_FALSE(::artm::core::InstanceManager::singleton().First() == nullptr);

  auto regularizer_config = test_mother.GenerateRegularizerConfig();
  auto model_config = test_mother.GenerateModelConfig();
  master->CreateOrReconfigureRegularizer(regularizer_config);
  master->CreateOrReconfigureModel(model_config);
  auto schema = ::artm::core::InstanceManager::singleton().First()->schema();
  EXPECT_TRUE(schema->has_model_config(model_config.name()));
  EXPECT_TRUE(schema->has_regularizer(regularizer_config.name()));
  master->DisposeModel(model_config.name());
  master->DisposeRegularizer(regularizer_config.name());

  artm::core::NodeControllerManager::singleton().Erase(node_id);
  EXPECT_EQ(master->clients_size(), 0);
  EXPECT_TRUE(::artm::core::InstanceManager::singleton().First() == nullptr);

  master.reset();
  artm::core::MasterComponentManager::singleton().Erase(master_id);
}
