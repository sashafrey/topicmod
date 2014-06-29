// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/node_controller_service_impl.h"

#include "boost/thread.hpp"

#include "glog/logging.h"

#include "artm/core/instance.h"
#include "artm/core/exceptions.h"

namespace artm {
namespace core {

NodeControllerServiceImpl::~NodeControllerServiceImpl() {
  if (instance_id_ != kUndefinedId) {
    artm::core::InstanceManager::singleton().Erase(instance_id_);
  }
}

void NodeControllerServiceImpl::CreateOrReconfigureInstance(
    const ::artm::MasterComponentConfig& request,
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

    instance->CreateOrReconfigureModel(request.config());
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

void NodeControllerServiceImpl::CreateOrReconfigureDictionary(
    const ::artm::core::CreateOrReconfigureDictionaryArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance == nullptr) {
      LOG(ERROR) << "Instance not found";
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance->CreateOrReconfigureDictionary(request.dictionary());
    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeDictionary(
    const ::artm::core::DisposeDictionaryArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance != nullptr) {
      instance->DisposeDictionary(request.dictionary_name());
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::ForcePullTopicModel(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance != nullptr) {
      instance->ForcePullTopicModel();
    } else {
      LOG(ERROR) << "No instances exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::ForcePushTopicModelIncrement(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto instance = artm::core::InstanceManager::singleton().Get(instance_id_);
    if (instance != nullptr) {
      instance->ForcePushTopicModelIncrement();
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
