// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/instance.h"

#include "boost/bind.hpp"

#include "artm/common.h"
#include "artm/processor.h"
#include "artm/template_manager.h"
#include "artm/topic_model.h"

namespace artm {
namespace core {

Instance::Instance(int id, const InstanceConfig& config)
    : lock_(),
      instance_id_(id),
      schema_(lock_, std::make_shared<InstanceSchema>(InstanceSchema(config))),
      next_model_id_(0),
      processor_queue_lock_(),
      processor_queue_(),
      merger_queue_lock_(),
      merger_queue_(),
      merger_(&merger_queue_lock_, &merger_queue_, &schema_),
      processors_() {
  Reconfigure(config);
}

Instance::~Instance() {}

int Instance::CreateModel(const ModelConfig& config) {
  int model_id = next_model_id_++;
  ReconfigureModel(model_id, config);
  return model_id;
}

int Instance::ReconfigureModel(int model_id, const ModelConfig& config) {
  merger_.UpdateModel(model_id, config);

  auto new_schema = schema_.get_copy();
  new_schema->set_model_config(model_id, std::make_shared<const ModelConfig>(config));
  schema_.set(new_schema);
  return ARTM_SUCCESS;
}

int Instance::DisposeModel(int model_id) {
  auto new_schema = schema_.get_copy();
  new_schema->discard_model(model_id);
  schema_.set(new_schema);

  merger_.DisposeModel(model_id);
  return ARTM_SUCCESS;
}

int Instance::Reconfigure(const InstanceConfig& config) {
  auto new_schema = schema_.get_copy();
  new_schema->set_instance_config(config);
  schema_.set(new_schema);

  // Adjust size of processors_; cast size to int to avoid compiler warning.
  while (static_cast<int>(processors_.size()) > config.processors_count()) processors_.pop_back();
  while (static_cast<int>(processors_.size()) < config.processors_count()) {
    processors_.push_back(
      std::shared_ptr<Processor>(new Processor(
        &processor_queue_lock_,
        &processor_queue_,
        &merger_queue_lock_,
        &merger_queue_,
        merger_,
        schema_)));
  }

  return ARTM_SUCCESS;
}

int Instance::RequestModelTopics(int model_id, ::artm::ModelTopics* model_topics) {
  std::shared_ptr<const ::artm::core::TopicModel> ttm = merger_.GetLatestTopicModel(model_id);
  int topics_size = ttm->topic_size();
  for (int token_index = 0; token_index < ttm->token_size(); token_index++) {
    TokenTopics* token_topics = model_topics->add_token_topic();
    token_topics->set_token(ttm->token(token_index));
    TopicWeightIterator iter = ttm->GetTopicWeightIterator(token_index);
    while (iter.NextTopic() < topics_size) {
      token_topics->add_topic_weight(iter.Weight());
    }
  }

  model_topics->set_items_processed(ttm->items_processed());
  for (int score_index = 0; score_index < ttm->score_size(); ++score_index) {
    model_topics->add_score(ttm->score(score_index));
  }

  return ARTM_SUCCESS;
}

int Instance::processor_queue_size() const {
  boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
  return processor_queue_.size();
}

int Instance::AddBatchIntoProcessorQueue(std::shared_ptr<const ProcessorInput> input) {
  boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
  processor_queue_.push(input);
  return ARTM_SUCCESS;
}

}  // namespace core
}  // namespace artm
