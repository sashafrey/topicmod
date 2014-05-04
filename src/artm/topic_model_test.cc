// Copyright 2014, Additive Regularization of Topic Models.

#include "gtest/gtest.h"

#include "artm/topic_model.h"
#include "artm/messages.pb.h"

TEST(TopicModelTest, Basic) {
  int no_topics = 3;
  int scores_count = 4;
  int no_tokens = 5;

  artm::core::TopicModel topic_model(no_topics, scores_count);
  topic_model.AddToken("token_1");
  topic_model.AddToken("token_2");
  topic_model.AddToken("token_3");
  topic_model.AddToken("token_4");
  topic_model.AddToken("token_5");

  //test 1
  float real_normalizer = 0;
  for (int i = 0; i < no_tokens; ++i) {
    for (int j = 0; j < no_topics; ++j) {
      topic_model.SetTokenWeight(i, j, 1);
    }
  }
  auto n_t = topic_model.GetTopicWeightIterator(0).GetNormalizer();
  for (int j = 0; j < no_topics; ++j) {
    real_normalizer += n_t[j];
  }
  float expected_normalizer = no_tokens * no_topics;   
  EXPECT_TRUE(std::abs(real_normalizer - expected_normalizer) < tolerance);

  //test 2
  real_normalizer = 0;
  for (int i = 0; i < no_tokens; ++i) {
    for (int j = 0; j < no_topics; ++j) {
      topic_model.SetRegularizerWeight(i, j, -0.5);
    }
  }
  n_t = topic_model.GetTopicWeightIterator(0).GetNormalizer();
  for (int j = 0; j < no_topics; ++j) {
    real_normalizer += n_t[j];
  }
  expected_normalizer = no_tokens * no_topics / 2.0;   
  EXPECT_TRUE(std::abs(real_normalizer - expected_normalizer) < tolerance);

  //test 3
  real_normalizer = 0;
  for (int i = 0; i < no_tokens; ++i) {
    for (int j = 0; j < no_topics; ++j) {
      topic_model.SetRegularizerWeight(i, j, -1.5);
    }
  }
  n_t = topic_model.GetTopicWeightIterator(0).GetNormalizer();
  for (int j = 0; j < no_topics; ++j) {
    real_normalizer += n_t[j];
  }
  expected_normalizer = 0;   
  EXPECT_TRUE(std::abs(real_normalizer - expected_normalizer) < tolerance);

  //test 4
  real_normalizer = 0;
  for (int i = 0; i < no_tokens; ++i) {
    for (int j = 0; j < no_topics; ++j) {
      topic_model.IncreaseTokenWeight(i, j, 0.4);
    }
  }
  n_t = topic_model.GetTopicWeightIterator(0).GetNormalizer();
  for (int j = 0; j < no_topics; ++j) {
    real_normalizer += n_t[j];
  }
  expected_normalizer = 0;   
  EXPECT_TRUE(std::abs(real_normalizer - expected_normalizer) < tolerance);

  //test 5
  real_normalizer = 0;
  for (int i = 0; i < no_tokens; ++i) {
    for (int j = 0; j < no_topics; ++j) {
      topic_model.IncreaseTokenWeight(i, j, 0.6);
    }
  }
  n_t = topic_model.GetTopicWeightIterator(0).GetNormalizer();
  for (int j = 0; j < no_topics; ++j) {
    real_normalizer += n_t[j];
  }
  expected_normalizer = no_tokens * no_topics / 2.0;   
  EXPECT_TRUE(std::abs(real_normalizer - expected_normalizer) < tolerance);

  //test 6
  real_normalizer = 0;
  for (int i = 0; i < no_tokens; ++i) {
    for (int j = 0; j < no_topics; ++j) {
      topic_model.SetTokenWeight(i, j, 1);
    }
  }
  n_t = topic_model.GetTopicWeightIterator(0).GetNormalizer();
  for (int j = 0; j < no_topics; ++j) {
    real_normalizer += n_t[j];
  }
  expected_normalizer = 0;   
  EXPECT_TRUE(std::abs(real_normalizer - expected_normalizer) < tolerance);

  //test 7
  real_normalizer = 0;
  for (int i = 0; i < no_tokens; ++i) {
    for (int j = 0; j < no_topics; ++j) {
      topic_model.SetRegularizerWeight(i, j, -0.5);
    }
  }
  n_t = topic_model.GetTopicWeightIterator(0).GetNormalizer();
  for (int j = 0; j < no_topics; ++j) {
    real_normalizer += n_t[j];
  }
  expected_normalizer = no_tokens * no_topics / 2.0;   
  EXPECT_TRUE(std::abs(real_normalizer - expected_normalizer) < tolerance);
}
