// Copyright 2014, Additive Regularization of Topic Models.

#include <vector>
#include <memory>

#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

#include "gtest/gtest.h"
#include "glog/logging.h"

#include "artm/memcached_server.h"
#include "artm/memcached_service.rpcz.h"
#include "artm/memcached_service.pb.h"
#include "rpcz/application.hpp"

// To run this particular test:
// artm_tests.exe --gtest_filter=MemcachedService.StartStop
TEST(MemcachedService, StartStop) {
  int server_id = artm::core::MemcachedServerManager::singleton().Create("tcp://*:5555");
  artm::core::MemcachedServerManager::singleton().Erase(server_id);
}

TEST(MemcachedService, StartConnectAndStop) {
  int server_id = artm::core::MemcachedServerManager::singleton().Create("tcp://*:5555");

  rpcz::application app(rpcz::application::options(1));
  artm::memcached::MemcachedService_Stub proxy(
    app.create_rpc_channel("tcp://localhost:5555"), true);

  artm::core::MemcachedServerManager::singleton().Erase(server_id);
}
