// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_MASTER_PROXY_H_
#define SRC_ARTM_CORE_MASTER_PROXY_H_

#include <string>

#include "artm/messages.pb.h"
#include "artm/core/master_interface.h"
#include "artm/core/template_manager.h"

namespace artm {
namespace core {

class MasterProxy : boost::noncopyable, public MasterInterface {
 public:
  ~MasterProxy();

  virtual int id() const;

  virtual void Reconfigure(const MasterComponentConfig& config);

  virtual void CreateOrReconfigureModel(const ModelConfig& config);
  virtual void DisposeModel(ModelName model_name);

  virtual void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  virtual void DisposeRegularizer(const std::string& name);

  virtual void CreateOrReconfigureDictionary(const DictionaryConfig& config);
  virtual void DisposeDictionary(const std::string& name);

  virtual void OverwriteTopicModel(const ::artm::TopicModel& topic_model);
  virtual bool RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model);
  virtual bool RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix);

  virtual void AddBatch(const Batch& batch);
  virtual void InvokeIteration(int iterations_count);
  virtual void WaitIdle();
  virtual void InvokePhiRegularizers();

 private:
  friend class TemplateManager<MasterInterface>;

  // All master components must be created via TemplateManager.
  MasterProxy(int id, const MasterProxyConfig& config);

  int id_;
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_CORE_MASTER_PROXY_H_
