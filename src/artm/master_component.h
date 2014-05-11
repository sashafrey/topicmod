// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MASTER_COMPONENT_H_
#define SRC_ARTM_MASTER_COMPONENT_H_

#include <string>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "rpcz/application.hpp"
#include "rpcz/rpc.hpp"
#include "rpcz/server.hpp"
#include "rpcz/service.hpp"

#include "artm/common.h"
#include "artm/data_loader.h"
#include "artm/instance.h"
#include "artm/master_component_service_impl.h"
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
  int clients_size() const { return clients_.size(); }

  // Retrieves topic model.
  // Returns true if succeeded, and false if model_id hasn't been found.
  bool RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model);

  // Reconfigures topic model if already exists, otherwise creates a new model.
  void ReconfigureModel(const ModelConfig& config);

  void DisposeModel(ModelId model_id);
  void Reconfigure(const MasterComponentConfig& config);

  void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  void DisposeRegularizer(const std::string& name);
  void InvokePhiRegularizers();

  void WaitIdle();
  void InvokeIteration(int iterations_count);
  void AddBatch(const Batch& batch);

 private:
  class ServiceEndpoint : boost::noncopyable {
   public:
    ServiceEndpoint(
        const std::string& endpoint,
        ThreadSafeCollectionHolder<std::string, NodeControllerService_Stub>* clients);

    ~ServiceEndpoint();
    std::string endpoint() const { return endpoint_; }

   private:
    std::string endpoint_;
    ThreadSafeCollectionHolder<std::string, NodeControllerService_Stub>* clients_;
    rpcz::application application_;

    // Keep all threads at the end of class members
    // (because the order of class members defines initialization order;
    // everything else should be initialized before creating threads).
    boost::thread thread_;

    void ThreadFunction();
  };

  friend class TemplateManager<MasterComponent, MasterComponentConfig>;

  // All master components must be created via TemplateManager.
  MasterComponent(int id, const MasterComponentConfig& config);

  mutable boost::mutex lock_;
  int master_id_;
  ThreadSafeHolder<MasterComponentConfig> config_;

  std::shared_ptr<Instance> local_instance_;
  std::shared_ptr<DataLoader> local_data_loader_;
  std::shared_ptr<ServiceEndpoint> service_endpoint_;
  ThreadSafeCollectionHolder<std::string, artm::core::NodeControllerService_Stub> clients_;
};

typedef TemplateManager<MasterComponent, MasterComponentConfig> MasterComponentManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_MASTER_COMPONENT_H_
