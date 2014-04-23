// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_INSTANCE_SCHEMA_H_
#define SRC_ARTM_INSTANCE_SCHEMA_H_

#include <map>
#include <memory>
#include <vector>

#include "artm/common.h"
#include "artm/messages.pb.h"
#include "artm/regularizer_interface.h"

namespace artm {
namespace core {

class InstanceSchema {
 public:
  InstanceSchema();
  explicit InstanceSchema(const InstanceSchema& schema);
  explicit InstanceSchema(const InstanceConfig& config);

  const InstanceConfig& instance_config() const;
  void set_instance_config(const InstanceConfig& instance_config);

  const ModelConfig& model_config(int id) const;
  void set_model_config(int id, const std::shared_ptr<const ModelConfig>& model_config);
  bool has_model_config(int id) const;
  void clear_model_config(int id);

  void set_regularizer(std::string name, 
    std::shared_ptr<RegularizerInterface>& regularizer_config);
  bool has_regularizer(std::string name) const;
  void clear_regularizer(std::string name);

  std::shared_ptr<std::map<std::string, std::shared_ptr<RegularizerInterface> >> 
    GetPointerToRegularizers();

  std::vector<int> GetModelIds() const;

 private:
  InstanceConfig instance_config_;
  std::map<int, std::shared_ptr<const ModelConfig> > models_config_;
  std::map<std::string, std::shared_ptr<RegularizerInterface> > regularizers_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_INSTANCE_SCHEMA_H_
