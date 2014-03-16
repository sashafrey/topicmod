// Copyright 2014, Additive Regularization of Topic Models.

#include <vector>
#include <memory>

#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

#include "gtest/gtest.h"

#include "artm/memcached_server.h"
#include "artm/memcached_service.rpcz.h"
#include "artm/memcached_service.pb.h"
#include "rpcz/application.hpp"

// To run this particular test:
// artm_tests.exe --gtest_filter=MemcachedService.*
TEST(MemcachedService, Basic) {
  // Start memcached server
  int server_id = artm::core::MemcachedServerManager::singleton().Create("tcp://*:5555");

  rpcz::application app;
  artm::memcached::MemcachedService_Stub proxy(
    app.create_rpc_channel("tcp://localhost:5555"), true);

  int timeout = 1000;

  artm::memcached::UpdateKeyArgs update_key_args;
  artm::memcached::UpdateKeyResult update_key_result;

  // Add first value
  update_key_args.set_key("the_key");
  update_key_args.set_key_group("the_key_group");
  update_key_args.add_value(10.0);
  update_key_args.add_value(13.0);
  proxy.UpdateKey(update_key_args, &update_key_result, timeout);
  EXPECT_EQ(update_key_result.error_code(), artm::memcached::kSuccess);
  EXPECT_EQ(update_key_result.value_size(), 2);
  EXPECT_EQ(update_key_result.value(0), 10.0);
  EXPECT_EQ(update_key_result.value(1), 13.0);

  // Add second value
  proxy.UpdateKey(update_key_args, &update_key_result, timeout);
  EXPECT_EQ(update_key_result.error_code(), artm::memcached::kSuccess);
  EXPECT_EQ(update_key_result.value_size(), 2);
  EXPECT_EQ(update_key_result.value(0), 20.0);
  EXPECT_EQ(update_key_result.value(1), 26.0);

  // Add message with too many values
  update_key_args.add_value(666.0);
  proxy.UpdateKey(update_key_args, &update_key_result, timeout);
  EXPECT_EQ(update_key_result.error_code(), artm::memcached::kInvalidArgument);

  artm::memcached::RetrieveGroupArgs retrieve_group_args;
  artm::memcached::RetrieveGroupResult retrieve_group_result;
  retrieve_group_args.set_key_group("no_such_key_group");
  proxy.RetrieveGroup(retrieve_group_args, &retrieve_group_result, timeout);
  EXPECT_EQ(retrieve_group_result.error_code(), artm::memcached::kKeyGroupNotFound);

  retrieve_group_args.set_key_group("the_key_group");
  proxy.RetrieveGroup(retrieve_group_args, &retrieve_group_result, timeout);
  EXPECT_EQ(retrieve_group_result.error_code(), artm::memcached::kSuccess);
  EXPECT_EQ(retrieve_group_result.key_size(), 1);
  EXPECT_EQ(retrieve_group_result.key(0), "the_key");

  artm::memcached::RetrieveKeyArgs retrieve_key_args;
  artm::memcached::RetrieveKeyResult retrieve_key_result;
  retrieve_key_args.set_key_group("no_such_key_group");
  retrieve_key_args.set_key("no_such_key");
  proxy.RetrieveKey(retrieve_key_args, &retrieve_key_result, timeout);
  EXPECT_EQ(retrieve_key_result.error_code(), artm::memcached::kKeyGroupNotFound);

  retrieve_key_args.set_key_group("the_key_group");
  retrieve_key_args.set_key("no_such_key");
  proxy.RetrieveKey(retrieve_key_args, &retrieve_key_result, timeout);
  EXPECT_EQ(retrieve_key_result.error_code(), artm::memcached::kKeyNotFound);

  retrieve_key_args.set_key("the_key");
  proxy.RetrieveKey(retrieve_key_args, &retrieve_key_result, timeout);
  EXPECT_EQ(retrieve_key_result.error_code(), artm::memcached::kSuccess);
  EXPECT_EQ(retrieve_key_result.value_size(), 2);
  EXPECT_EQ(retrieve_key_result.value(0), 20.0);
  EXPECT_EQ(retrieve_key_result.value(1), 26.0);

  // Stop memcached server.
  artm::core::MemcachedServerManager::singleton().Erase(server_id);
}

void StressTestThreadFunction() {
  rpcz::application app;
  artm::memcached::MemcachedService_Stub proxy(
    app.create_rpc_channel("tcp://localhost:5555"), true);

  int timeout = 1000;
  artm::memcached::UpdateKeyArgs update_key_args;
  artm::memcached::UpdateKeyResult update_key_result;
  update_key_args.set_key_group("the_key_group");
  update_key_args.add_value(10.0f);
  update_key_args.add_value(13.0f);
  for (int i = 0; i < 100; ++i) {
    update_key_args.set_key(std::string("the_key") + boost::lexical_cast<std::string>(i % 100));
    proxy.UpdateKey(update_key_args, &update_key_result, timeout);
  }
}

TEST(MemcachedService, Stress) {
  int server_id = artm::core::MemcachedServerManager::singleton().Create("tcp://*:5555");
  std::vector<std::shared_ptr<boost::thread> > threads;
  int thread_size = 4;
  for (int i = 0; i < thread_size; ++i) {
    threads.push_back(std::make_shared<boost::thread>(&StressTestThreadFunction));
  }

  for (int i = 0; i < thread_size; ++i) {
    threads[i]->join();
  }
}
