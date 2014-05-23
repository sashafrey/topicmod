// Copyright 2014, Additive Regularization of Topic Models.

#include <string>

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

void Instance::ReconfigureModel(const ModelConfig& config) {
  merger_.UpdateModel(config);

  auto new_schema = schema_.get_copy();
  new_schema->set_model_config(config.model_id(), std::make_shared<const ModelConfig>(config));
  schema_.set(new_schema);
}

void Instance::DisposeModel(ModelId model_id) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_model_config(model_id);
  schema_.set(new_schema);

  merger_.DisposeModel(model_id);
}

void Instance::CreateOrReconfigureRegularizer(const std::string& name,
  std::shared_ptr<regularizer::RegularizerInterface> regularizer) {

  auto new_schema = schema_.get_copy();
  new_schema->set_regularizer(name, regularizer);
  schema_.set(new_schema);
}

void Instance::DisposeRegularizer(const std::string& name) {
  auto new_schema = schema_.get_copy();
  new_schema->clear_regularizer(name);
  schema_.set(new_schema);
}

void Instance::ForceResetScores(ModelId model_id) {
  merger_.ForceResetScores(model_id);
}

void Instance::ForceSyncWithMemcached(ModelId model_id) {
  merger_.ForceSyncWithMemcached(model_id);
}

void Instance::InvokePhiRegularizers() {
  merger_.InvokePhiRegularizers();
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

bool Instance::RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model) {
  std::shared_ptr<const ::artm::core::TopicModel> ttm = merger_.GetLatestTopicModel(model_id);
  if (ttm == nullptr) return false;
  ttm->RetrieveExternalTopicModel(topic_model);
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
