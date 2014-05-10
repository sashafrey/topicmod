// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MASTER_COMPONENT_H_
#define SRC_ARTM_MASTER_COMPONENT_H_

#include <string>

#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "artm/common.h"
#include "artm/data_loader.h"
#include "artm/instance.h"
#include "artm/messages.pb.h"
#include "artm/regularizer_interface.h"
#include "artm/template_manager.h"
#include "artm/topic_model.h"

namespace artm {
namespace core {

class MasterComponent : boost::noncopyable {
 public:
  ~MasterComponent();

  int id() const;
  bool isInLocalModusOperandi() const;
  bool isInNetworkModusOperandi() const;

  // Retrieves topic model.
  // Returns true if succeeded, and false if model_id hasn't been found.
  bool RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model);

  // Reconfigures topic model if already exists, otherwise creates a new model.
  void ReconfigureModel(const ModelConfig& config);

  void DisposeModel(ModelId model_id);
  void Reconfigure(const MasterComponentConfig& config);

  void CreateOrReconfigureRegularizer(const std::string& name,
                                      std::shared_ptr<RegularizerInterface> regularizer);
  void DisposeRegularizer(const std::string& name);
  void InvokePhiRegularizers();

  void WaitIdle();
  void InvokeIteration(int iterations_count);
  void AddBatch(const Batch& batch);

 private:
  friend class TemplateManager<MasterComponent, MasterComponentConfig>;

  // All master components must be created via TemplateManager.
  MasterComponent(int id, const MasterComponentConfig& config);

  mutable boost::mutex lock_;
  int master_id_;
  ThreadSafeHolder<MasterComponentConfig> config_;

  std::shared_ptr<Instance> local_instance_;
  std::shared_ptr<DataLoader> local_data_loader_;
};

typedef TemplateManager<MasterComponent, MasterComponentConfig> MasterComponentManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_MASTER_COMPONENT_H_
