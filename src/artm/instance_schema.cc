// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/instance_schema.h"

namespace artm {
namespace core {

InstanceSchema::InstanceSchema() : instance_config_(), models_config_(), regularizers_() {}

InstanceSchema::InstanceSchema(const InstanceSchema& schema)
    : instance_config_(schema.instance_config_),
      models_config_(schema.models_config_),
      regularizers_(schema.regularizers_) {}

InstanceSchema::InstanceSchema(const InstanceConfig& config)
    : instance_config_(config), models_config_(), regularizers_() {}

void InstanceSchema::set_instance_config(const InstanceConfig& instance_config) {
  instance_config_.CopyFrom(instance_config);
}

const InstanceConfig& InstanceSchema::instance_config() const {
  return instance_config_;
}

void InstanceSchema::set_model_config(
    int id, const std::shared_ptr<const ModelConfig>& model_config) {
  auto iter = models_config_.find(id);
  if (iter != models_config_.end()) {
    iter->second = model_config;
  } else {
    models_config_.insert(std::make_pair(id, model_config));
  }
}

const ModelConfig& InstanceSchema::model_config(int id) const {
  auto iter = models_config_.find(id);
  return *(iter->second);
}

bool InstanceSchema::has_model_config(int id) const {
  auto iter = models_config_.find(id);
  return iter != models_config_.end();
}

void InstanceSchema::clear_model_config(int id) {
  auto iter = models_config_.find(id);
  if (iter != models_config_.end()) {
    models_config_.erase(iter);
  }
}

void InstanceSchema::set_regularizer(const std::string& name,
                                     const std::shared_ptr<regularizer::RegularizerInterface>& regularizer) {
  auto iter = regularizers_.find(name);
  if (iter != regularizers_.end()) {
    iter->second = regularizer;
  } else {
    regularizers_.insert(std::make_pair(name, regularizer));
  }
}

bool InstanceSchema::has_regularizer(const std::string& name) const {
  auto iter = regularizers_.find(name);
  return iter != regularizers_.end();
}
void InstanceSchema::clear_regularizer(const std::string name) {
  auto iter = regularizers_.find(name);
  if (iter != regularizers_.end()) {
    regularizers_.erase(iter);
  }
}

std::shared_ptr<regularizer::RegularizerInterface> InstanceSchema::regularizer(const std::string& name) {
  auto iter = regularizers_.find(name);
  if (iter != regularizers_.end()) {
    return iter->second;
  } else {
    return nullptr;
  }
}

std::vector<int> InstanceSchema::GetModelIds() const {
  std::vector<int> retval;
  for (auto iter = models_config_.begin(); iter != models_config_.end(); ++iter) {
    retval.push_back(iter->first);
  }

  return retval;
}

}  // namespace core
}  // namespace artm
