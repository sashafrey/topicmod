// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/master_component.h"

#include "glog/logging.h"

#include "artm/exceptions.h"
#include "artm/helpers.h"

namespace artm {
namespace core {

MasterComponent::MasterComponent(int id, const MasterComponentConfig& config)
    : lock_(),
      master_id_(id),
      config_(lock_, std::make_shared<MasterComponentConfig>(MasterComponentConfig(config))),
      local_instance_(nullptr),
      local_data_loader_(nullptr),
      service_endpoint_(nullptr),
      clients_(lock_) {
  Reconfigure(config);
}

MasterComponent::~MasterComponent() {
  if (local_instance_ != nullptr) {
    artm::core::InstanceManager::singleton().Erase(local_instance_->id());
    local_instance_.reset();
  }

  if (local_data_loader_ != nullptr) {
    artm::core::DataLoaderManager::singleton().Erase(local_data_loader_->id());
    local_data_loader_.reset();
  }

  if (service_endpoint_ != nullptr) {
    service_endpoint_.reset();
  }
}

int MasterComponent::id() const {
  return master_id_;
}

bool MasterComponent::isInLocalModusOperandi() const {
  return config_.get()->modus_operandi() == MasterComponentConfig_ModusOperandi_Local;
}

bool MasterComponent::isInNetworkModusOperandi() const {
  return config_.get()->modus_operandi() == MasterComponentConfig_ModusOperandi_Network;
}

void MasterComponent::ReconfigureModel(const ModelConfig& config) {
  if (isInLocalModusOperandi()) {
    local_instance_->ReconfigureModel(config);
    return;
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::DisposeModel(ModelId model_id) {
  if (isInLocalModusOperandi()) {
    local_instance_->DisposeModel(model_id);
    return;
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  if (isInLocalModusOperandi()) {
    local_instance_->CreateOrReconfigureRegularizer(config);
    return;
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::DisposeRegularizer(const std::string& name) {
  if (isInLocalModusOperandi()) {
    local_instance_->DisposeRegularizer(name);
    return;
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::InvokePhiRegularizers() {
  if (isInLocalModusOperandi()) {
    local_instance_->InvokePhiRegularizers();
    return;
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::Reconfigure(const MasterComponentConfig& config) {
  config_.set(std::make_shared<MasterComponentConfig>(config));
  if (isInLocalModusOperandi()) {
    if (service_endpoint_ != nullptr) {
      service_endpoint_.reset();
    }

    if (local_instance_ == nullptr) {
      int instance_id = artm::core::InstanceManager::singleton().Create(config.instance_config());
      local_instance_ = artm::core::InstanceManager::singleton().Get(instance_id);
    } else {
      local_instance_->Reconfigure(config.instance_config());
    }

    DataLoaderConfig data_loader_config(config.data_loader_config());
    data_loader_config.set_instance_id(local_instance_->id());
    if (local_data_loader_ == nullptr) {
      int data_loader_id = artm::core::DataLoaderManager::singleton().Create(data_loader_config);
      local_data_loader_ = artm::core::DataLoaderManager::singleton().Get(data_loader_id);
    } else {
      local_data_loader_->Reconfigure(data_loader_config);
    }

    return;
  }

  if (isInNetworkModusOperandi()) {
    if (local_instance_ != nullptr) {
      artm::core::InstanceManager::singleton().Erase(local_instance_->id());
      local_instance_.reset();
    }

    if (local_data_loader_ != nullptr) {
      artm::core::DataLoaderManager::singleton().Erase(local_data_loader_->id());
      local_data_loader_.reset();
    }

    if (service_endpoint_ == nullptr) {
      service_endpoint_.reset(new ServiceEndpoint(config_.get()->service_endpoint(), &clients_));
    }

    return;
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

bool MasterComponent::RequestTopicModel(ModelId model_id, ::artm::TopicModel* topic_model) {
  if (isInLocalModusOperandi()) {
    return local_instance_->RequestTopicModel(model_id, topic_model);
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::WaitIdle() {
  if (isInLocalModusOperandi()) {
    return local_data_loader_->WaitIdle();
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::InvokeIteration(int iterations_count) {
  if (isInLocalModusOperandi()) {
    return local_data_loader_->InvokeIteration(iterations_count);
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

void MasterComponent::AddBatch(const Batch& batch) {
  if (isInLocalModusOperandi()) {
    return local_data_loader_->AddBatch(batch);
  }

  if (isInNetworkModusOperandi()) {
    BOOST_THROW_EXCEPTION(NotImplementedException("MasterComponent - network modus operandi"));
  }

  BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("MasterComponent::modus_operandi"));
}

MasterComponent::ServiceEndpoint::~ServiceEndpoint() {
  application_.terminate();
  thread_.join();
}

MasterComponent::ServiceEndpoint::ServiceEndpoint(
    const std::string& endpoint,
    ThreadSafeCollectionHolder<std::string, NodeControllerService_Stub>* clients)
    : endpoint_(endpoint), clients_(clients),
      application_(rpcz::application::options(1)), thread_() {
  boost::thread t(&MasterComponent::ServiceEndpoint::ThreadFunction, this);
  thread_.swap(t);
}

void MasterComponent::ServiceEndpoint::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "MasterComponent");
    LOG(INFO) << "Establishing MasterComponentService on " << endpoint();
    rpcz::server server(application_);
    ::artm::core::MasterComponentServiceImpl master_component_service_impl(clients_);
    server.register_service(&master_component_service_impl);
    server.bind(endpoint());
    application_.run();
    LOG(INFO) << "MasterComponentService on " << endpoint() << " is stopped.";
  } catch(...) {
    LOG(FATAL) << "Fatal exception in MasterComponent::ServiceEndpoint::ThreadFunction() function";
    return;
  }
}

}  // namespace core
}  // namespace artm
