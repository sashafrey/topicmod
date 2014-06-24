// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_MASTER_COMPONENT_H_
#define SRC_ARTM_CORE_MASTER_COMPONENT_H_

#include <functional>
#include <string>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "rpcz/application.hpp"
#include "rpcz/rpc.hpp"
#include "rpcz/server.hpp"
#include "rpcz/service.hpp"

#include "artm/messages.pb.h"

#include "artm/core/common.h"
#include "artm/core/master_component_service_impl.h"
#include "artm/core/internals.pb.h"
#include "artm/core/template_manager.h"

namespace zmq {
class context_t;
}  // namespace zmq

namespace artm {

class RegularizerInterface;

namespace core {

class ClientInterface;
class LocalClient;
class NetworkClientCollection;
class DataLoader;
class LocalDataLoader;
class RemoteDataLoader;
class Instance;
class TopicModel;

class MasterComponent : boost::noncopyable {
 public:
  ~MasterComponent();

  int id() const;
  bool isInLocalModusOperandi() const;
  bool isInNetworkModusOperandi() const;
  int clients_size() const;

  // Retrieves topic model.
  // Returns true if succeeded, and false if model_name hasn't been found.
  bool RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model);
  bool RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix);

  // Reconfigures topic model if already exists, otherwise creates a new model.
  void ReconfigureModel(const ModelConfig& config);
  void OverwriteTopicModel(const ::artm::TopicModel& topic_model);

  void DisposeModel(ModelName model_name);
  void Reconfigure(const MasterComponentConfig& config);

  void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  void DisposeRegularizer(const std::string& name);
  void InvokePhiRegularizers();

  void WaitIdle();
  void InvokeIteration(int iterations_count);
  void AddBatch(const Batch& batch);

  static DataLoaderConfig ExtractdDataLoaderConfig(const MasterComponentConfig& config,
                                                   int instance_id);
  static InstanceConfig ExtractInstanceConfig(const MasterComponentConfig& config);

  // Throws InvalidOperation exception if new config is invalid.
  void ValidateConfig(const MasterComponentConfig& config);

 private:
  class ServiceEndpoint : boost::noncopyable {
   public:
    ServiceEndpoint(const std::string& endpoint, NetworkClientCollection* clients);
    ~ServiceEndpoint();
    std::string endpoint() const { return endpoint_; }
    MasterComponentServiceImpl* impl() { return &master_component_service_impl_; }

   private:
    std::string endpoint_;
    NetworkClientCollection* clients_;
    std::unique_ptr<zmq::context_t> zeromq_context_;
    std::unique_ptr<rpcz::application> application_;
    MasterComponentServiceImpl master_component_service_impl_;

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

  // Endpoint for clients to talk with master component
  std::shared_ptr<ServiceEndpoint> service_endpoint_;

  std::shared_ptr<ClientInterface> client_interface_;
};

typedef TemplateManager<MasterComponent, MasterComponentConfig> MasterComponentManager;

// Common interface that share operations, applicable in both modus operandi (Local and Network).
class ClientInterface {
 public:
  virtual ~ClientInterface() {}

  virtual void ReconfigureModel(const ModelConfig& config) = 0;
  virtual void DisposeModel(ModelName model_name) = 0;

  virtual void CreateOrReconfigureRegularizer(const RegularizerConfig& config) = 0;
  virtual void DisposeRegularizer(const std::string& name) = 0;
  virtual void InvokePhiRegularizers() = 0;

  virtual void Reconfigure(const MasterComponentConfig& config) = 0;
};

class LocalClient : public ClientInterface {
 public:
  LocalClient() {}
  virtual ~LocalClient();

  virtual void ReconfigureModel(const ModelConfig& config);
  virtual void DisposeModel(ModelName model_name);

  virtual void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  virtual void DisposeRegularizer(const std::string& name);
  virtual void InvokePhiRegularizers();
  virtual void OverwriteTopicModel(const ::artm::TopicModel& topic_model);

  virtual void Reconfigure(const MasterComponentConfig& config);

  bool RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model);
  bool RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix);
  void WaitIdle();
  void InvokeIteration(int iterations_count);
  void AddBatch(const Batch& batch);

 private:
  std::shared_ptr<Instance> local_instance_;
  std::shared_ptr<LocalDataLoader> local_data_loader_;
};

class NetworkClientCollection : public ClientInterface {
 public:
  NetworkClientCollection(boost::mutex& lock) : lock_(lock), clients_(lock_) {}  // NOLINT
  virtual ~NetworkClientCollection();

  virtual void ReconfigureModel(const ModelConfig& config);
  virtual void DisposeModel(ModelName model_name);

  virtual void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  virtual void DisposeRegularizer(const std::string& name);
  virtual void InvokePhiRegularizers();

  virtual void Reconfigure(const MasterComponentConfig& config);

  bool ConnectClient(std::string endpoint, rpcz::application* application);
  bool DisconnectClient(std::string endpoint);

  int clients_size() const {
    return clients_.size();
  }

  void for_each_client(std::function<void(artm::core::NodeControllerService_Stub&)> f);
  void for_each_endpoint(std::function<void(std::string)> f);

 private:
  boost::mutex& lock_;
  ThreadSafeCollectionHolder<std::string, artm::core::NodeControllerService_Stub> clients_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_MASTER_COMPONENT_H_
