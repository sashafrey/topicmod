// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/lexical_cast.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"

#include "gtest/gtest.h"

#include "artm/instance.h"
#include "artm/data_loader.h"
#include "artm/messages.pb.h"
#include "artm/protobuf_helpers.h"

class InstanceTest : boost::noncopyable {
 public:
  std::shared_ptr<artm::core::DataLoader> data_loader() { return data_loader_; }
  std::shared_ptr<artm::core::Instance> instance() { return instance_; }

  InstanceTest() {
    int instance_id = artm::core::InstanceManager::singleton().Create(artm::InstanceConfig());
    instance_ = artm::core::InstanceManager::singleton().Get(instance_id);

    artm::DataLoaderConfig data_loader_config;
    data_loader_config.set_instance_id(instance_id);
    int data_loader_id = artm::core::DataLoaderManager::singleton().Create(data_loader_config);
    data_loader_ = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  }

  ~InstanceTest() {
    artm::core::InstanceManager::singleton().Erase(instance_->id());
    artm::core::DataLoaderManager::singleton().Erase(data_loader_->id());
  }

  // Some way of generating a junk content..
  // If you call this, then you really shouldn't care which content it will be;
  // the only promise of this function is to generate a batch that will have fixed
  // number of items (nItems). Under normal parameters it will
  // also have nTokens of unique tokens, and each item won't exceed maxLength.
  std::shared_ptr<artm::Batch> GenerateBatch(int n_tokens, int n_items, int start_id,
                                             int max_length, int max_occurences) {
    std::shared_ptr<artm::Batch> batch(std::make_shared<artm::Batch>());
    for (int i = 0; i < n_tokens; ++i) {
      std::stringstream str;
      str << "token" << i;
      batch->add_token(str.str());
    }

    int iToken = 0;
    int iLength = 0;
    int iOccurences = 0;

    for (int iItem = 0; iItem < n_items; ++iItem) {
      artm::Item* item = batch->add_item();
      item->set_id(start_id++);
      artm::Field* field = item->add_field();
      for (int i = 0; i <= iLength; ++i) {
        field->add_token_id(iToken);
        field->add_token_count(iOccurences + 1);

        iOccurences = (iOccurences + 1) % max_occurences;
        iToken = (iToken + 1) % n_tokens;
      }

      iLength = (iLength + 1) % max_length;
    }

    return batch;
  }

 private:
  std::shared_ptr<artm::core::DataLoader> data_loader_;
  std::shared_ptr<artm::core::Instance> instance_;
};

// artm_tests.exe --gtest_filter=Instance.*
TEST(Instance, Basic) {
  int instance_id = artm::core::InstanceManager::singleton().Create(artm::InstanceConfig());
  std::shared_ptr<artm::core::Instance> instance =
    artm::core::InstanceManager::singleton().Get(instance_id);

  artm::DataLoaderConfig data_loader_config;
  data_loader_config.set_instance_id(instance_id);
  int data_loader_id = artm::core::DataLoaderManager::singleton().Create(data_loader_config);
  std::shared_ptr<artm::core::DataLoader> data_loader =
    artm::core::DataLoaderManager::singleton().Get(data_loader_id);

  artm::Batch batch1;
  batch1.add_token("first token");
  batch1.add_token("second");
  for (int i = 0; i < 2; ++i) {
    artm::Item* item = batch1.add_item();
    artm::Field* field = item->add_field();
    field->add_token_id(i);
    field->add_token_count(i+1);
  }

  data_loader->AddBatch(batch1);  // +2

  for (int iBatch = 0; iBatch < 2; ++iBatch) {
    artm::Batch batch;
    for (int i = 0; i < (3 + iBatch); ++i) batch.add_item();  // +3, +4
    data_loader->AddBatch(batch);
  }

  EXPECT_EQ(data_loader->GetTotalItemsCount(), 9);

  data_loader->AddBatch(batch1);  // +2
  EXPECT_EQ(data_loader->GetTotalItemsCount(), 11);

  artm::Batch batch4;
  batch4.add_token("second");
  batch4.add_token("last");
  artm::Item* item = batch4.add_item();
  artm::Field* field = item->add_field();
  for (int iToken = 0; iToken < 2; ++iToken) {
    field->add_token_id(iToken);
    field->add_token_count(iToken + 2);
  }

  data_loader->AddBatch(batch4);

  artm::ModelConfig config;
  config.set_enabled(true);
  config.set_topics_count(3);
  artm::core::ModelId model_id =
    boost::lexical_cast<std::string>(boost::uuids::random_generator()());
  config.set_model_id(boost::lexical_cast<std::string>(model_id));
  instance->ReconfigureModel(config);

  data_loader->InvokeIteration(20);
  data_loader->WaitIdle();

  config.set_enabled(false);
  instance->ReconfigureModel(config);

  artm::TopicModel topic_model;
  instance->RequestTopicModel(model_id, &topic_model);
  EXPECT_EQ(topic_model.token_size(), 3);
  EXPECT_TRUE(artm::core::model_has_token(topic_model, "first token"));
  EXPECT_TRUE(artm::core::model_has_token(topic_model, "second"));
  EXPECT_TRUE(artm::core::model_has_token(topic_model, "last"));
  EXPECT_FALSE(artm::core::model_has_token(topic_model, "of cource!"));

  artm::core::InstanceManager::singleton().Erase(instance_id);
  artm::core::DataLoaderManager::singleton().Erase(data_loader_id);
}

TEST(Instance, MultipleStreamsAndModels) {
  InstanceTest test;

  // This setting will ensure that
  // - first model have  Token0, Token2, Token4,
  // - second model have Token1, Token3, Token6,
  auto batch = test.GenerateBatch(6, 6, 0, 1, 1);
  test.data_loader()->AddBatch(*batch);

  artm::DataLoaderConfig config;
  config.set_instance_id(test.instance()->id());
  artm::Stream* s1 = config.add_stream();
  s1->set_type(artm::Stream_Type_ItemIdModulus);
  s1->set_modulus(2);
  s1->add_residuals(0);
  s1->set_name("train");
  artm::Stream* s2 = config.add_stream();
  s2->set_type(artm::Stream_Type_ItemIdModulus);
  s2->set_modulus(2);
  s2->add_residuals(1);
  s2->set_name("test");
  test.data_loader()->Reconfigure(config);

  artm::ModelConfig m1;
  m1.set_stream_name("train");
  m1.set_enabled(true);
  m1.set_model_id(boost::lexical_cast<std::string>(boost::uuids::random_generator()()));
  artm::Score* score = m1.add_score();
  score->set_type(artm::Score_Type_Perplexity);

  // In the little synthetic dataset created below
  // tokens in 'train' and 'test' sample won't overlap.
  // If we chose to calc perplexity on test sample
  // it will be zero, because none of test-sample tokens
  // are present in token-topic-matrix. Therefore,
  // using train sample to get non-zero perplexity score.
  score->set_stream_name("train");
  test.instance()->ReconfigureModel(m1);

  artm::ModelConfig m2;
  m2.set_stream_name("test");
  m2.set_enabled(true);
  m2.set_model_id(boost::lexical_cast<std::string>(boost::uuids::random_generator()()));
  test.instance()->ReconfigureModel(m2);

  for (int iter = 0; iter < 100; ++iter) {
  test.data_loader()->InvokeIteration(1);
    test.data_loader()->WaitIdle();
  }


  artm::TopicModel m1t;
  test.instance()->RequestTopicModel(m1.model_id(), &m1t);

  artm::TopicModel m2t;
  test.instance()->RequestTopicModel(m2.model_id(), &m2t);

  // Verification for m1t (the first model)
  EXPECT_TRUE(artm::core::model_has_token(m1t, "token0"));
  EXPECT_TRUE(artm::core::model_has_token(m1t, "token2"));
  EXPECT_TRUE(artm::core::model_has_token(m1t, "token4"));

  // if model has other tokens, their Phi weight should be at zero.
  for (int token_index = 0; token_index < m1t.token_size(); ++token_index) {
    std::string token = m1t.token(token_index);
    if ((token == "token1") || (token == "token3") || (token == "token5")) {
      for (int topic_index = 0; topic_index < m1t.topics_count(); ++topic_index) {
        // todo(alfrey) Verification was disabled because now all tokens are initialized with random values.
        // EXPECT_EQ(m1t.token_weights(token_index).value(topic_index), 0);
      }
    }
  }

  // Verification for m2t (the second model)
  EXPECT_TRUE(artm::core::model_has_token(m2t, "token1"));
  EXPECT_TRUE(artm::core::model_has_token(m2t, "token3"));
  EXPECT_TRUE(artm::core::model_has_token(m2t, "token5"));

  // if model has other tokens, their Phi weight should be at zero.
  for (int token_index = 0; token_index < m2t.token_size(); ++token_index) {
    std::string token = m2t.token(token_index);
    if ((token == "token0") || (token == "token2") || (token == "token4")) {
      for (int topic_index = 0; topic_index < m2t.topics_count(); ++topic_index) {
        // todo(alfrey) Verification was disabled because now all tokens are initialized with random values.
        // EXPECT_EQ(m2t.token_weights(token_index).value(topic_index), 0);
      }
    }
  }

  EXPECT_EQ(m1t.scores().value_size(), 1);
  EXPECT_GT(m1t.scores().value(0), 0);
}
