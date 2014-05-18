// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MASTER_COMPONENT_H_
#define SRC_ARTM_MASTER_COMPONENT_H_

#include <functional>
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

namespace zmq {
class context_t;
}  // namespace zmq

namespace artm {
namespace core {

class ClientInterface;
class LocalClient;
class NetworkClientCollection;

class MasterComponent : boost::noncopyable {
 public:
  ~MasterComponent();

  int id() const;
  bool isInLocalModusOperandi() const;
  bool isInNetworkModusOperandi() const;
  int clients_size() const;

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
    ServiceEndpoint(const std::string& endpoint, NetworkClientCollection* clients);
    ~ServiceEndpoint();
    std::string endpoint() const { return endpoint_; }

   private:
    std::string endpoint_;
    NetworkClientCollection* clients_;
    std::unique_ptr<zmq::context_t> zeromq_context_;
    std::unique_ptr<rpcz::application> application_;

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
  virtual void DisposeModel(ModelId model_id) = 0;

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
  virtual void DisposeModel(ModelId model_id);

  virtual void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  virtual void DisposeRegularizer(const std::string& name);
  virtual void InvokePhiRegularizers();

  virtual void Reconfigure(const MasterComponentConfig& config);

  bool RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model);
  void WaitIdle();
  void InvokeIteration(int iterations_count);
  void AddBatch(const Batch& batch);

 private:
  std::shared_ptr<Instance> local_instance_;
  std::shared_ptr<DataLoader> local_data_loader_;
};

class NetworkClientCollection : public ClientInterface {
 public:
  NetworkClientCollection(boost::mutex& lock) : lock_(lock), clients_(lock_) {}  // NOLINT
  virtual ~NetworkClientCollection();

  virtual void ReconfigureModel(const ModelConfig& config);
  virtual void DisposeModel(ModelId model_id);

  virtual void CreateOrReconfigureRegularizer(const RegularizerConfig& config);
  virtual void DisposeRegularizer(const std::string& name);
  virtual void InvokePhiRegularizers();

  virtual void Reconfigure(const MasterComponentConfig& config);

  bool ConnectClient(std::string endpoint, rpcz::application* application);
  bool DisconnectClient(std::string endpoint);

  int clients_size() const {
    return clients_.size();
  }

 private:
  void for_each_client(std::function<void(artm::core::NodeControllerService_Stub&)> f);
  void for_each_endpoint(std::function<void(std::string)> f);
  mutable boost::mutex& lock_;
  ThreadSafeCollectionHolder<std::string, artm::core::NodeControllerService_Stub> clients_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_MASTER_COMPONENT_H_
