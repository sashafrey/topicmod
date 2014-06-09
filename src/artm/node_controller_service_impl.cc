// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/node_controller_service_impl.h"

#include "boost/thread.hpp"

#include "glog/logging.h"

#include "artm/instance.h"
#include "artm/data_loader.h"
#include "artm/exceptions.h"

namespace artm {
namespace core {

NodeControllerServiceImpl::~NodeControllerServiceImpl() {
  if (instance_id_ != kUndefinedId) {
    artm::core::InstanceManager::singleton().Erase(instance_id_);
  }

  if (data_loader_id_ != kUndefinedId) {
    artm::core::DataLoaderManager::singleton().Erase(data_loader_id_);
  }
}

void NodeControllerServiceImpl::CreateOrReconfigureInstance(
    const ::artm::core::InstanceConfig& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance != nullptr) {
        instance->Reconfigure(request);
    } else {
      instance_id_ = artm::core::InstanceManager::singleton().Create(request);
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeInstance(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  artm::core::InstanceManager::singleton().Erase(instance_id_);
  instance_id_ = kUndefinedId;
  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureDataLoader(
    const ::artm::core::DataLoaderConfig& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto data_loader = artm::core::DataLoaderManager::singleton().Get(data_loader_id_);
    if (data_loader != nullptr) {
      data_loader->Reconfigure(request);
    } else {
      DataLoaderConfig data_loader_config(request);
      data_loader_config.set_instance_id(instance_id_);
      DataLoaderManager& dlm = artm::core::DataLoaderManager::singleton();
      data_loader_id_ = dlm.Create<RemoteDataLoader>(data_loader_config);
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeDataLoader(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  artm::core::DataLoaderManager::singleton().Erase(data_loader_id_);
  data_loader_id_ = kUndefinedId;
  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureModel(
    const ::artm::core::CreateOrReconfigureModelArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance == nullptr) {
      LOG(ERROR) << "Instance not found";
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance->ReconfigureModel(request.config());
    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeModel(
    const ::artm::core::DisposeModelArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
  if (instance != nullptr) {
    instance->DisposeModel(request.model_name());
  }

  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureRegularizer(
    const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance == nullptr) {
      LOG(ERROR) << "Instance not found";
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance->CreateOrReconfigureRegularizer(request.config());
    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeRegularizer(
    const ::artm::core::DisposeRegularizerArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
  if (instance != nullptr) {
    instance->DisposeRegularizer(request.regularizer_name());
  }

  response.send(Void());
}

void NodeControllerServiceImpl::ForceSyncWithMemcached(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance != nullptr) {
      instance->ForceSyncWithMemcached(ModelName());
    } else {
      LOG(ERROR) << "No instances exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

}  // namespace core
}  // namespace artm
