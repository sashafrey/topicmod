// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/master_component.h"

#include <vector>

#include "glog/logging.h"
#include "zmq.hpp"

#include "artm/exceptions.h"
#include "artm/helpers.h"
#include "artm/zmq_context.h"

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

void MasterComponent::DisposeModel(ModelId model_id) {
  client_interface_->DisposeModel(model_id);
}

void MasterComponent::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  client_interface_->CreateOrReconfigureRegularizer(config);
}

void MasterComponent::DisposeRegularizer(const std::string& name) {
  client_interface_->DisposeRegularizer(name);
}

void MasterComponent::InvokePhiRegularizers() {
  client_interface_->InvokePhiRegularizers();
}

void MasterComponent::Reconfigure(const MasterComponentConfig& config) {
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

bool MasterComponent::RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model) {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    return local_client->RequestTopicModel(model_id, topic_model);
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
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
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
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::AddBatch(const Batch& batch) {
  if (isInLocalModusOperandi()) {
    LocalClient* local_client = dynamic_cast<LocalClient*>(client_interface_.get());
    return local_client->AddBatch(batch);
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

MasterComponent::ServiceEndpoint::~ServiceEndpoint() {
  application_->terminate();
  thread_.join();
}

MasterComponent::ServiceEndpoint::ServiceEndpoint(
    const std::string& endpoint, NetworkClientCollection* clients)
    : endpoint_(endpoint), clients_(clients), application_(nullptr), thread_() {
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
    ::artm::core::MasterComponentServiceImpl master_component_service_impl(clients_);
    server.register_service(&master_component_service_impl);
    server.bind(endpoint());
    application_->run();
    LOG(INFO) << "MasterComponentService on " << endpoint() << " is stopped.";
  } catch(...) {
    LOG(FATAL) << "Fatal exception in MasterComponent::ServiceEndpoint::ThreadFunction() function";
    return;
  }
}

DataLoaderConfig MasterComponent::ExtractdDataLoaderConfig(
    const MasterComponentConfig& config, int instance_id) {
  DataLoaderConfig retval;
  retval.set_instance_id(instance_id);

  if (config.has_disk_path()) {
    retval.set_disk_path(config.disk_path());
  }

  if (config.has_processor_queue_max_size()) {
    retval.set_queue_size(config.processor_queue_max_size());
  }

  retval.mutable_stream()->CopyFrom(config.stream());

  if (config.has_compact_batches()) {
    retval.set_compact_batches(config.compact_batches());
  }

  if (config.has_cache_processor_output()) {
    retval.set_cache_processor_output(config.cache_processor_output());
  }

  if (config.has_master_component_connect_endpoint()) {
    retval.set_master_component_endpoint(config.master_component_connect_endpoint());
  }

  return retval;
}

InstanceConfig MasterComponent::ExtractInstanceConfig(
    const MasterComponentConfig& config) {
  InstanceConfig retval;

  if (config.has_processors_count()) {
    retval.set_processors_count(config.processors_count());
  }

  if (config.has_master_component_connect_endpoint()) {
    retval.set_master_component_endpoint(config.master_component_connect_endpoint());
  }

  if (config.has_merger_queue_max_size()) {
    retval.set_merger_queue_max_size(config.merger_queue_max_size());
  }

  return retval;
}

void LocalClient::ReconfigureModel(const ModelConfig& config) {
  local_instance_->ReconfigureModel(config);
}

void LocalClient::DisposeModel(ModelId model_id) {
  local_instance_->DisposeModel(model_id);
}

void LocalClient::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  local_instance_->CreateOrReconfigureRegularizer(config);
}

void LocalClient::DisposeRegularizer(const std::string& name) {
  local_instance_->DisposeRegularizer(name);
}

void LocalClient::InvokePhiRegularizers() {
  local_instance_->InvokePhiRegularizers();
}

void LocalClient::Reconfigure(const MasterComponentConfig& config) {
  if (local_instance_ == nullptr) {
    int instance_id = artm::core::InstanceManager::singleton().Create(
      MasterComponent::ExtractInstanceConfig(config));
    local_instance_ = artm::core::InstanceManager::singleton().Get(instance_id);
  } else {
    local_instance_->Reconfigure(MasterComponent::ExtractInstanceConfig(config));
  }

  DataLoaderConfig data_loader_config(
    MasterComponent::ExtractdDataLoaderConfig(config, local_instance_->id()));
  if (local_data_loader_ == nullptr) {
    int data_loader_id = artm::core::DataLoaderManager::singleton().Create(data_loader_config);
    local_data_loader_ = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
  } else {
    local_data_loader_->Reconfigure(data_loader_config);
  }
}

LocalClient::~LocalClient() {
  if (local_instance_ != nullptr) {
    artm::core::InstanceManager::singleton().Erase(local_instance_->id());
    local_instance_.reset();
  }

  if (local_data_loader_ != nullptr) {
    artm::core::DataLoaderManager::singleton().Erase(local_data_loader_->id());
    local_data_loader_.reset();
  }
}

bool LocalClient::RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model) {
  return local_instance_->RequestTopicModel(model_id, topic_model);
}

void LocalClient::WaitIdle() {
  local_data_loader_->WaitIdle();
}

void LocalClient::InvokeIteration(int iterations_count) {
  local_data_loader_->InvokeIteration(iterations_count);
}

void LocalClient::AddBatch(const Batch& batch) {
  local_data_loader_->AddBatch(batch);
}

void NetworkClientCollection::ReconfigureModel(const ModelConfig& config) {
  for_each_client([&](NodeControllerService_Stub& client) {
    CreateOrReconfigureModelArgs args;
    args.mutable_config()->CopyFrom(config);
    Void response;
    client.CreateOrReconfigureModel(args, &response);
  });
}


void NetworkClientCollection::DisposeModel(ModelId model_id) {
  for_each_client([&](NodeControllerService_Stub& client) {
    DisposeModelArgs args;
    args.set_model_id(model_id);
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

void NetworkClientCollection::InvokePhiRegularizers() {
  BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
}

void NetworkClientCollection::Reconfigure(const MasterComponentConfig& config) {
  for_each_client([&](NodeControllerService_Stub& client) {
    Void response;
    client.CreateOrReconfigureDataLoader(
      MasterComponent::ExtractdDataLoaderConfig(config, UnknownId), &response);

    client.CreateOrReconfigureInstance(MasterComponent::ExtractInstanceConfig(config), &response);
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