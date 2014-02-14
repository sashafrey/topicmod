#include "gtest/gtest.h"

#include <boost/thread/mutex.hpp>

#include "artm/instance.cc"
#include "artm/data_loader.h"
#include "artm/messages.pb.h"
#include "artm/protobuf_helpers.h"

using namespace ::artm;
using namespace ::artm::core;

class InstanceTest : boost::noncopyable {
private:
  std::shared_ptr<DataLoader> data_loader_;
  std::shared_ptr<Instance> instance_;
public:
  std::shared_ptr<DataLoader> data_loader() { return data_loader_; }
  std::shared_ptr<Instance> instance() { return instance_; }

  InstanceTest() {
    int instance_id = InstanceManager::singleton().Create(0, InstanceConfig());
    instance_ = InstanceManager::singleton().Get(instance_id);

    DataLoaderConfig data_loader_config;
    data_loader_config.set_instance_id(instance_id);
    int data_loader_id = DataLoaderManager::singleton().Create(0, data_loader_config);
    data_loader_ = DataLoaderManager::singleton().Get(data_loader_id);
  }

  ~InstanceTest() {
    InstanceManager::singleton().Erase(instance_->id());
    DataLoaderManager::singleton().Erase(data_loader_->id());
  }

  // Some way of generating a junk content.. 
  // If you call this, then you really shouldn't care which content it will be;
  // the only promise of this function is to generate a batch that will have fixed
  // number of items (nItems). Under normal parameters it will
  // also have nTokens of unique tokens, and each item won't exceed maxLength.
  std::shared_ptr<Batch> GenerateBatch(int nTokens, int nItems, int startId, int maxLength, int maxOccurences) {
    std::shared_ptr<Batch> batch(std::make_shared<Batch>());
    for (int i = 0; i < nTokens; ++i) {
      std::stringstream str;
      str << "token" << i;
      batch->add_token(str.str());
    }

    int iToken = 0;
    int iLength = 0;
    int iOccurences = 0;

    for (int iItem = 0; iItem < nItems; ++iItem) {
      Item* item = batch->add_item();
      item->set_id(startId++);
      Field* field = item->add_field();
      for (int i = 0; i <= iLength; ++i) {
        field->add_token_id(iToken);
        field->add_token_count(iOccurences + 1);
        
        iOccurences = (iOccurences + 1) % maxOccurences;
        iToken = (iToken + 1) % nTokens;
      }

      iLength = (iLength + 1) % maxLength;
    }

    return batch;
  }
};

// artm_tests.exe --gtest_filter=Instance.*
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
  
  ModelConfig config;
  config.set_enabled(true);
  config.set_topics_count(3);
  int model_id = instance->CreateModel(config);

  data_loader->InvokeIteration(20);
  instance->WaitModelProcessed(model_id, 150);

  config.set_enabled(false);
  instance->ReconfigureModel(model_id, config);

  ModelTopics model_topics;
  instance->RequestModelTopics(model_id, &model_topics);
  EXPECT_EQ(model_topics.token_topic_size(), 3);
  EXPECT_TRUE(model_has_token(model_topics, "first token"));
  EXPECT_TRUE(model_has_token(model_topics, "second"));
  EXPECT_TRUE(model_has_token(model_topics, "last"));
  EXPECT_FALSE(model_has_token(model_topics, "of cource!"));
  
 /* int found = 0;
  for (int i = 0; i < model_topics.token_topic_size(); ++i) {
    std::string token = model_topics.token_topic(i).token();    
    if (token == "first token") { found++; continue; }
    if (token == "second") { found++; continue; }
    if (token == "last") { found++; continue; }
  }*/

//  EXPECT_EQ(found, 3);

  InstanceManager::singleton().Erase(instance_id);
  DataLoaderManager::singleton().Erase(data_loader_id);
}

TEST(Instance, MultipleStreamsAndModels) {
  InstanceTest test;

  // This setting will ensure that 
  // - first model have  Token0, Token2, Token4,
  // - second model have Token1, Token3, Token6,
  auto batch = test.GenerateBatch(6, 6, 0, 1, 1);
  test.data_loader()->AddBatch(*batch);
  
  DataLoaderConfig config;
  config.set_instance_id(test.instance()->id());
  Stream* s1 = config.add_stream();
  s1->set_type(Stream_Type::Stream_Type_ItemIdModulus);
  s1->set_modulus(2);
  s1->add_residuals(0);
  s1->set_name("train");
  Stream* s2 = config.add_stream();
  s2->set_type(Stream_Type::Stream_Type_ItemIdModulus);
  s2->set_modulus(2);
  s2->add_residuals(1);
  s2->set_name("test");
  test.data_loader()->Reconfigure(config);

  ModelConfig m1;
  m1.set_stream_name("train");
  m1.set_enabled(true);
  Score* score = m1.add_score();
  score->set_type(Score_Type::Score_Type_Perplexity);
  score->set_stream_name("test");
  int m1_id = test.instance()->CreateModel(m1);

  ModelConfig m2;
  m2.set_stream_name("test");
  m2.set_enabled(true);
  int m2_id = test.instance()->CreateModel(m2);

  test.data_loader()->InvokeIteration(2);
  test.instance()->WaitModelProcessed(m1_id, 6);
  test.instance()->WaitModelProcessed(m2_id, 6);

  ModelTopics m1t;
  test.instance()->RequestModelTopics(m1_id, &m1t);

  ModelTopics m2t;
  test.instance()->RequestModelTopics(m2_id, &m2t);

  EXPECT_EQ(m1t.token_topic_size(), 3);
  EXPECT_EQ(m2t.token_topic_size(), 3);

  EXPECT_TRUE(model_has_token(m1t, "token0"));
  EXPECT_TRUE(model_has_token(m1t, "token2"));
  EXPECT_TRUE(model_has_token(m1t, "token4"));

  EXPECT_TRUE(model_has_token(m2t, "token1"));
  EXPECT_TRUE(model_has_token(m2t, "token3"));
  EXPECT_TRUE(model_has_token(m2t, "token5"));

  EXPECT_EQ(m1t.score_size(), 1);
  EXPECT_TRUE(m1t.score(0) > 0);
}