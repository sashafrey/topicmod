// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/node_controller_service_impl.h"

#include "boost/thread.hpp"

#include "artm/instance.h"
#include "artm/data_loader.h"
#include "artm/exceptions.h"

namespace artm {
namespace core {

void NodeControllerServiceImpl::CreateOrReconfigureInstance(
    const ::artm::InstanceConfig& request,
    ::rpcz::reply< ::artm::core::Int> response) {
  try {
    auto instance = artm::core::InstanceManager::singleton().Get(request.instance_id());
    int instance_id;
    if (instance != nullptr) {
        instance->Reconfigure(request);
        instance_id = instance->id();
    } else {
      instance_id = artm::core::InstanceManager::singleton().Create(request);
    }

    Int reply;
    reply.set_value(instance_id);
    response.send(reply);
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeInstance(
    const ::artm::core::Int& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  artm::core::InstanceManager::singleton().Erase(request.value());
}

void NodeControllerServiceImpl::CreateOrReconfigureDataLoader(
    const ::artm::DataLoaderConfig& request,
    ::rpcz::reply< ::artm::core::Int> response) {
  try {
    auto data_loader = artm::core::DataLoaderManager::singleton().Get(request.instance_id());
    int data_loader_id;
    if (data_loader != nullptr) {
        data_loader->Reconfigure(request);
        data_loader_id = data_loader->id();
    } else {
      data_loader_id = artm::core::DataLoaderManager::singleton().Create(request);
    }

    Int reply;
    reply.set_value(data_loader_id);
    response.send(reply);
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeDataLoader(
    const ::artm::core::Int& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  artm::core::DataLoaderManager::singleton().Erase(request.value());
}

void NodeControllerServiceImpl::CreateOrReconfigureModel(
    const ::artm::core::CreateOrReconfigureModelArgs& request,
    ::rpcz::reply< ::artm::core::Int> response) {
  try {
    auto instance = artm::core::InstanceManager::singleton().Get(request.instance_id());
    if (instance == nullptr) {
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance->ReconfigureModel(request.config());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeModel(
    const ::artm::core::DisposeModelArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  auto instance = artm::core::InstanceManager::singleton().Get(request.instance_id());
  if (instance != nullptr) {
    instance->DisposeModel(request.model_id());
  }
}

void NodeControllerServiceImpl::CreateOrReconfigureRegularizer(
    const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
    ::rpcz::reply< ::artm::core::Int> response) {
  try {
    auto instance = artm::core::InstanceManager::singleton().Get(request.instance_id());
    if (instance == nullptr) {
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance->CreateOrReconfigureRegularizer(request.config());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeRegularizer(
    const ::artm::core::DisposeRegularizerArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  auto instance = artm::core::InstanceManager::singleton().Get(request.instance_id());
  if (instance != nullptr) {
    instance->DisposeRegularizer(request.regularizer_name());
  }
}

}  // namespace core
}  // namespace artm
