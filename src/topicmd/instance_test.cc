#include "gtest/gtest.h"

#include <boost/thread/mutex.hpp>

#include "topicmd/instance.cc"
#include "topicmd/data_loader.h"
#include "topicmd/messages.pb.h"

using namespace ::artm;
using namespace ::artm::core;

// topicmd_tests.exe --gtest_filter=Instance.*
TEST(Instance, Basic) {
  int instance_id = InstanceManager::singleton().Create(0, InstanceConfig());
  std::shared_ptr<Instance> instance = InstanceManager::singleton().Get(instance_id);

  DataLoaderConfig data_loader_config;
  data_loader_config.set_instance_id(instance_id);
  int data_loader_id = DataLoaderManager::singleton().Create(0, data_loader_config);
  std::shared_ptr<DataLoader> data_loader = DataLoaderManager::singleton().Get(data_loader_id);
  
  Batch batch1;
  batch1.add_token("first token");
  batch1.add_token("second");
  for (int i = 0; i < 2; ++i) {
    Item* item = batch1.add_item();
    Field* field = item->add_field();
    field->add_token_id(i);
    field->add_token_count(i+1);
  }

  data_loader->AddBatch(batch1); // +2
  
  for (int iBatch = 0; iBatch < 2; ++iBatch) {
    Batch batch;
    for (int i = 0; i < (3 + iBatch); ++i) batch.add_item(); // +3, +4
    data_loader->AddBatch(batch);
  }

  EXPECT_EQ(data_loader->GetTotalItemsCount(), 9);

  data_loader->AddBatch(batch1);  // +2
  EXPECT_EQ(data_loader->GetTotalItemsCount(), 11); 

  Batch batch4;
  batch4.add_token("second");
  batch4.add_token("last");
  Item* item = batch4.add_item();
  Field* field = item->add_field();
  for (int iToken = 0; iToken < 2; ++iToken) {
    field->add_token_id(iToken);
    field->add_token_count(iToken + 2);
  }

  data_loader->AddBatch(batch4);
  
  int model_id = 0;
  ModelConfig config;
  config.set_enabled(true);
  config.set_topics_count(3);
  instance->UpdateModel(model_id, config);

  instance->WaitModelProcessed(model_id, 150);

  config.set_enabled(false);
  instance->UpdateModel(model_id, config);

  ModelTopics model_topics;
  instance->RequestModelTopics(model_id, &model_topics);
  EXPECT_EQ(model_topics.token_topic_size(), 3);
  int found = 0;
  for (int i = 0; i < model_topics.token_topic_size(); ++i) {
    std::string token = model_topics.token_topic(i).token();    
    if (token == "first token") { found++; continue; }
    if (token == "second") { found++; continue; }
    if (token == "last") { found++; continue; }
  }

  EXPECT_EQ(found, 3);
}
