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
      next_model_id_(1),
      application_(),
      memcached_service_proxy_(lock_, nullptr),
      processor_queue_lock_(),
      processor_queue_(),
      merger_queue_lock_(),
      merger_queue_(),
      merger_(&merger_queue_lock_, &merger_queue_, &schema_, &memcached_service_proxy_),
      processors_() {
  Reconfigure(config);
}

Instance::~Instance() {}

int Instance::CreateModel(const ModelConfig& config) {
  int model_id = next_model_id_++;
  ReconfigureModel(model_id, config);
  return model_id;
}

void Instance::ReconfigureModel(int model_id, const ModelConfig& config) {
  merger_.UpdateModel(model_id, config);

  auto new_schema = schema_.get_copy();
  new_schema->set_model_config(model_id, std::make_shared<const ModelConfig>(config));
  schema_.set(new_schema);
}

void Instance::DisposeModel(int model_id) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_model_config(model_id);
  schema_.set(new_schema);

  merger_.DisposeModel(model_id);
}

void Instance::CreateRegularizer(std::string name, 
                                  std::shared_ptr<RegularizerInterface> regularizer) {
  ReconfigureRegularizer(name, regularizer);
}

void Instance::ReconfigureRegularizer(std::string name, std::shared_ptr<RegularizerInterface> regularizer) {
  auto new_schema = schema_.get_copy();
  new_schema->set_regularizer(name, regularizer);
  schema_.set(new_schema);
}

void Instance::DisposeRegularizer(std::string name) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_regularizer(name);
  schema_.set(new_schema);
}

void Instance::Reconfigure(const InstanceConfig& config) {
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

  // Recreate memcached_service_proxy_;
  if (config.has_memcached_endpoint()) {
    std::shared_ptr<artm::memcached::MemcachedService_Stub> new_ptr(
      new artm::memcached::MemcachedService_Stub(
        application_.create_rpc_channel(config.memcached_endpoint()), true));
    memcached_service_proxy_.set(new_ptr);
  } else {
    memcached_service_proxy_.set(nullptr);
  }
}

bool Instance::RequestModelTopics(int model_id, ::artm::ModelTopics* model_topics) {
  std::shared_ptr<const ::artm::core::TopicModel> ttm = merger_.GetLatestTopicModel(model_id);
  if (ttm == nullptr) return false;

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

  return true;
}

int Instance::processor_queue_size() const {
  boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
  return processor_queue_.size();
}

void Instance::AddBatchIntoProcessorQueue(std::shared_ptr<const ProcessorInput> input) {
  boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
  processor_queue_.push(input);
}

}  // namespace core
}  // namespace artm
