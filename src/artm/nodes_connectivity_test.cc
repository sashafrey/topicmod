// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/thread.hpp"

#include "gtest/gtest.h"

#include "artm/node_controller.h"
#include "artm/master_component.h"
#include "artm/messages.pb.h"

// artm_tests.exe --gtest_filter=NodesConnectivityTest.*
TEST(NodesConnectivityTest, Basic) {
  ::artm::MasterComponentConfig master_config;
  master_config.set_modus_operandi(::artm::MasterComponentConfig_ModusOperandi_Network);
  master_config.set_service_endpoint("tcp://*:5555");
  int master_id = artm::core::MasterComponentManager::singleton().Create(master_config);
  auto master = artm::core::MasterComponentManager::singleton().Get(master_id);
  ASSERT_EQ(master->clients_size(), 0);

  ::artm::NodeControllerConfig node_config;
  node_config.set_master_component_connect_endpoint("tcp://localhost:5555");
  node_config.set_node_controller_create_endpoint("tcp://*:5556");
  node_config.set_node_controller_connect_endpoint("tcp://localhost:5556");
  int node_id = artm::core::NodeControllerManager::singleton().Create(node_config);
  ASSERT_EQ(master->clients_size(), 1);

  artm::core::NodeControllerManager::singleton().Erase(node_id);
  ASSERT_EQ(master->clients_size(), 0);

  master.reset();
  artm::core::MasterComponentManager::singleton().Erase(master_id);
}
