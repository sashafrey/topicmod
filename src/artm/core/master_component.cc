// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/master_component.h"

#include <vector>

#include "glog/logging.h"
#include "zmq.hpp"

#include "artm/regularizer_interface.h"

#include "artm/core/exceptions.h"
#include "artm/core/helpers.h"
#include "artm/core/zmq_context.h"
#include "artm/core/generation.h"
#include "artm/core/data_loader.h"
#include "artm/core/instance.h"
#include "artm/core/topic_model.h"

namespace artm {
namespace core {

MasterComponent::MasterComponent(int id, const MasterComponentConfig& config)
    : lock_(),
      master_id_(id),
      config_(lock_, std::make_shared<MasterComponentConfig>(MasterComponentConfig(config))),
      service_endpoint_(nullptr),
      client_interface_(nullptr) {
  Reconfigure(config);
}

MasterComponent::~MasterComponent() {
}

int MasterComponent::id() const {
  return master_id_;
}

int MasterComponent::clients_size() const {
  return dynamic_cast<NetworkClientCollection*>(client_interface_.get())->clients_size();
}

bool MasterComponent::isInLocalModusOperandi() const {
  return config_.get()->modus_operandi() == MasterComponentConfig_ModusOperandi_Local;
}

bool MasterComponent::isInNetworkModusOperandi() const {
  return config_.get()->modus_operandi() == MasterComponentConfig_ModusOperandi_Network;
}

void MasterComponent::ReconfigureModel(const ModelConfig& config) {
  client_interface_->ReconfigureModel(config);
}

void MasterComponent::DisposeModel(ModelName model_name) {
  client_interface_->DisposeModel(model_name);
}

void MasterComponent::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  client_interface_->CreateOrReconfigureRegularizer(config);
}

void MasterComponent::DisposeRegularizer(const std::string& name) {
  client_interface_->DisposeRegularizer(name);
}

void MasterComponent::CreateOrReconfigureDictionary(const DictionaryConfig& config) {
  client_interface_->CreateOrReconfigureDictionary(config);
}

void MasterComponent::DisposeDictionary(const std::string& name) {
  client_interface_->DisposeDictionary(name);
}

void MasterComponent::InvokePhiRegularizers() {
  client_interface_->InvokePhiRegularizers();
}

void MasterComponent::Reconfigure(const MasterComponentConfig& config) {
  ValidateConfig(config);

  auto previous_modus_operandi = config_.get()->modus_operandi();
  auto new_modus_operandi = config.modus_operandi();

  config_.set(std::make_shared<MasterComponentConfig>(config));

  if ((new_modus_operandi == previous_modus_operandi) && (client_interface_ != nullptr)) {
    client_interface_->Reconfigure(config);
    return;
  }

  if (new_modus_operandi == MasterComponentConfig_ModusOperandi_Local) {
    service_endpoint_.reset();
    client_interface_.reset(new LocalClient());
    client_interface_->Reconfigure(config);
    return;
  }

  if (new_modus_operandi == MasterComponentConfig_ModusOperandi_Network) {
    client_interface_.reset(new NetworkClientCollection(lock_));
    service_endpoint_.reset(
      new ServiceEndpoint(config_.get()->master_component_create_endpoint(),
                          dynamic_cast<NetworkClientCollection*>(client_interface_.get())));
    client_interface_->Reconfigure(config);
    return;
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

bool MasterComponent::RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    return local_client->RequestTopicModel(model_name, topic_model);
  }

  if (isInNetworkModusOperandi()) {
    return service_endpoint_->impl()->RequestTopicModel(model_name, topic_model);
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::OverwriteTopicModel(const ::artm::TopicModel& topic_model) {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    local_client->OverwriteTopicModel(topic_model);
    return;
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("OverwriteTopicModel in network mode"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

bool MasterComponent::RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix) {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    return local_client->RequestThetaMatrix(model_name, theta_matrix);
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::WaitIdle() {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    local_client->WaitIdle();
    return;
  }

  if (isInNetworkModusOperandi()) {
    service_endpoint_->impl()->WaitIdle();
    return;
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::InvokeIteration(int iterations_count) {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    local_client->InvokeIteration(iterations_count);
    return;
  }

  if (isInNetworkModusOperandi()) {
    service_endpoint_->impl()->InvokeIteration(iterations_count, config_.get()->disk_path());
    return;
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::AddBatch(const Batch& batch) {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    return local_client->AddBatch(batch);
  }

  if (isInNetworkModusOperandi()) {
    Generation::SaveBatch(batch, config_.get()->disk_path());
    return;
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

MasterComponent::ServiceEndpoint::~ServiceEndpoint() {
  application_->terminate();
  thread_.join();
}

MasterComponent::ServiceEndpoint::ServiceEndpoint(
    const std::string& endpoint, NetworkClientCollection* clients)
    : endpoint_(endpoint), clients_(clients), application_(nullptr),
      master_component_service_impl_(clients_), thread_() {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));
  boost::thread t(&MasterComponent::ServiceEndpoint::ThreadFunction, this);
  thread_.swap(t);
}

void MasterComponent::ServiceEndpoint::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "MasterComponent");
    LOG(INFO) << "Establishing MasterComponentService on " << endpoint();
    rpcz::server server(*application_);
    server.register_service(&master_component_service_impl_);
    server.bind(endpoint());
    application_->run();
    LOG(INFO) << "MasterComponentService on " << endpoint() << " is stopped.";
  } catch(...) {
    LOG(FATAL) << "Fatal exception in MasterComponent::ServiceEndpoint::ThreadFunction() function";
    return;
  }
}


void MasterComponent::ValidateConfig(const MasterComponentConfig& config) {
  if (config.modus_operandi() == MasterComponentConfig_ModusOperandi_Network) {
    if (!config.has_master_component_connect_endpoint() ||
        !config.has_master_component_create_endpoint()) {
      BOOST_THROW_EXCEPTION(InvalidOperation(
        "Network modus operandi require all endpoints to be set."));
    }

    if (!config.has_disk_path()) {
      BOOST_THROW_EXCEPTION(InvalidOperation(
        "Network modus operandi require disk_path to be set."));
    }
  }
}

void LocalClient::ReconfigureModel(const ModelConfig& config) {
  local_instance_->ReconfigureModel(config);
}

void LocalClient::DisposeModel(ModelName model_name) {
  local_instance_->DisposeModel(model_name);
}

void LocalClient::OverwriteTopicModel(const ::artm::TopicModel& topic_model) {
  local_instance_->OverwriteTopicModel(topic_model);
}

void LocalClient::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  local_instance_->CreateOrReconfigureRegularizer(config);
}

void LocalClient::DisposeRegularizer(const std::string& name) {
  local_instance_->DisposeRegularizer(name);
}

void LocalClient::CreateOrReconfigureDictionary(const DictionaryConfig& config) {
  local_instance_->CreateOrReconfigureDictionary(config);
}

void LocalClient::DisposeDictionary(const std::string& name) {
  local_instance_->DisposeDictionary(name);
}

void LocalClient::InvokePhiRegularizers() {
  local_instance_->InvokePhiRegularizers();
}

void LocalClient::Reconfigure(const MasterComponentConfig& config) {
  if (local_instance_ == nullptr) {
    int instance_id = artm::core::InstanceManager::singleton().Create(config);
    local_instance_ = artm::core::InstanceManager::singleton().Get(instance_id);
  } else {
    local_instance_->Reconfigure(config);
  }
}

LocalClient::~LocalClient() {
  if (local_instance_ != nullptr) {
    artm::core::InstanceManager::singleton().Erase(local_instance_->id());
    local_instance_.reset();
  }
}

bool LocalClient::RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) {
  return local_instance_->RequestTopicModel(model_name, topic_model);
}

bool LocalClient::RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix) {
  return local_instance_->local_data_loader()->RequestThetaMatrix(model_name, theta_matrix);
}

void LocalClient::WaitIdle() {
  local_instance_->local_data_loader()->WaitIdle();
}

void LocalClient::InvokeIteration(int iterations_count) {
  local_instance_->local_data_loader()->InvokeIteration(iterations_count);
}

void LocalClient::AddBatch(const Batch& batch) {
  local_instance_->local_data_loader()->AddBatch(batch);
}

void NetworkClientCollection::ReconfigureModel(const ModelConfig& config) {
  for_each_client([&](NodeControllerService_Stub& client) {
    CreateOrReconfigureModelArgs args;
    args.mutable_config()->CopyFrom(config);
    Void response;
    client.CreateOrReconfigureModel(args, &response);
  });
}


void NetworkClientCollection::DisposeModel(ModelName model_name) {
  for_each_client([&](NodeControllerService_Stub& client) {
    DisposeModelArgs args;
    args.set_model_name(model_name);
    Void response;
    client.DisposeModel(args, &response);
  });
}

void NetworkClientCollection::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  for_each_client([&](NodeControllerService_Stub& client) {
    CreateOrReconfigureRegularizerArgs args;
    args.mutable_config()->CopyFrom(config);
    Void response;
    client.CreateOrReconfigureRegularizer(args, &response);
  });
}

void NetworkClientCollection::DisposeRegularizer(const std::string& name) {
  for_each_client([&](NodeControllerService_Stub& client) {
    DisposeRegularizerArgs args;
    args.set_regularizer_name(name);
    Void response;
    client.DisposeRegularizer(args, &response);
  });
}

void NetworkClientCollection::CreateOrReconfigureDictionary(const DictionaryConfig& config) {
  for_each_client([&](NodeControllerService_Stub& client) {
    CreateOrReconfigureDictionaryArgs args;
    args.mutable_dictionary()->CopyFrom(config);
    Void response;
    client.CreateOrReconfigureDictionary(args, &response);
  });
}

void NetworkClientCollection::DisposeDictionary(const std::string& name) {
  for_each_client([&](NodeControllerService_Stub& client) {
    DisposeDictionaryArgs args;
    args.set_dictionary_name(name);
    Void response;
    client.DisposeDictionary(args, &response);
  });
}

void NetworkClientCollection::InvokePhiRegularizers() {
  BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
}

void NetworkClientCollection::Reconfigure(const MasterComponentConfig& config) {
  for_each_client([&](NodeControllerService_Stub& client) {
    Void response;
    client.CreateOrReconfigureInstance(config, &response);
  });
}

NetworkClientCollection::~NetworkClientCollection() {
  for_each_endpoint([&](std::string endpoint) {
    DisconnectClient(endpoint);
  });
}

bool NetworkClientCollection::ConnectClient(std::string endpoint, rpcz::application* application) {
  if (clients_.has_key(endpoint)) {
    LOG(ERROR) << "Unable to connect client " << endpoint << ", client already exists.";
    return false;
  }

  std::shared_ptr<NodeControllerService_Stub> client(
    new artm::core::NodeControllerService_Stub(
      application->create_rpc_channel(endpoint), true));
  clients_.set(endpoint, client);
  return true;
}

bool NetworkClientCollection::DisconnectClient(std::string endpoint) {
  if (!clients_.has_key(endpoint)) {
    LOG(ERROR) << "Unable to disconnect client " << endpoint << ", client is not connected.";
    return false;
  }

  clients_.erase(endpoint);
  return true;
}

void NetworkClientCollection::for_each_client(
    std::function<void(artm::core::NodeControllerService_Stub&)> f) {
  std::vector<std::string> client_ids(clients_.keys());
  for (auto &client_id : client_ids) {
    f(*clients_.get(client_id));
  }
}

void NetworkClientCollection::for_each_endpoint(
    std::function<void(std::string)> f) {
  std::vector<std::string> client_ids(clients_.keys());
  for (auto &client_id : client_ids) {
    f(client_id);
  }
}

}  // namespace core
}  // namespace artm
