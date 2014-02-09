#ifndef ARTM_INSTANCE_SCHEMA_
#define ARTM_INSTANCE_SCHEMA_

#include <map>
#include <memory>

#include "artm/messages.pb.h"

namespace artm { namespace core {

class InstanceSchema {
  private:
    InstanceConfig instance_config_;
    std::map<int, std::shared_ptr<const ModelConfig> > models_config_;
  public:
    InstanceSchema() : instance_config_(), models_config_()
    {
    }

    InstanceSchema(const InstanceSchema& schema) :
        instance_config_(schema.instance_config_), 
        models_config_(schema.models_config_) 
    {
    }

    InstanceSchema(const InstanceConfig& config) :
        instance_config_(config), 
        models_config_() 
    {
    }

    void set_instance_config(const InstanceConfig& instance_config) {
      instance_config_.CopyFrom(instance_config);
    }

    const InstanceConfig& get_instance_config() const {
      return instance_config_;
    }

    void set_model_config(int id, const std::shared_ptr<const ModelConfig>& model_config) {
      auto iter = models_config_.find(id);
      if (iter != models_config_.end()) {
        iter->second = model_config;
      } else {
        models_config_.insert(std::make_pair(id, model_config));
      }
    }

    const ModelConfig& get_model_config(int id) const {
      auto iter = models_config_.find(id);
      return *(iter->second);
    }

    bool has_model_config(int id) const {
      auto iter = models_config_.find(id);
      return iter != models_config_.end();
    }

    void discard_model(int id) {
      auto iter = models_config_.find(id);
      if (iter != models_config_.end()) {
        models_config_.erase(iter);
      }
    }

    std::vector<int> get_model_ids() const {
      std::vector<int> retval;
      for (auto iter = models_config_.begin(); iter != models_config_.end(); ++iter) {
        retval.push_back(iter->first);
      }

      return retval;
    }
  };

}} // namespace artm/core

#endif // ARTM_INSTANCE_SCHEMA_ 
