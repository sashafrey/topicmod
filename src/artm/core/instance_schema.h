// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_INSTANCE_SCHEMA_H_
#define SRC_ARTM_CORE_INSTANCE_SCHEMA_H_

#include <map>
#include <memory>
#include <vector>
#include <string>

#include "artm/core/common.h"
#include "artm/core/internals.pb.h"

namespace artm {

class RegularizerInterface;
class ModelConfig;

namespace core {

class InstanceSchema {
 public:
  InstanceSchema();
  explicit InstanceSchema(const InstanceSchema& schema);
  explicit InstanceSchema(const InstanceConfig& config);

  const InstanceConfig& instance_config() const;
  void set_instance_config(const InstanceConfig& instance_config);

  const ModelConfig& model_config(ModelName id) const;
  void set_model_config(ModelName id, const std::shared_ptr<const ModelConfig>& model_config);
  bool has_model_config(ModelName id) const;
  void clear_model_config(ModelName id);

  void set_regularizer(const std::string& name,
                       const std::shared_ptr<RegularizerInterface>& regularizer);
  bool has_regularizer(const std::string& name) const;
  void clear_regularizer(const std::string name);

  std::shared_ptr<RegularizerInterface> regularizer(const std::string& name);

  std::vector<ModelName> GetModelNames() const;

 private:
  InstanceConfig instance_config_;
  std::map<std::string, std::shared_ptr<RegularizerInterface> > regularizers_;
  std::map<ModelName, std::shared_ptr<const ModelConfig> > models_config_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_INSTANCE_SCHEMA_H_
