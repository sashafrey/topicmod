// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/node_controller_service_impl.h"

#include "boost/thread.hpp"

#include "glog/logging.h"

#include "artm/core/instance.h"
#include "artm/core/exceptions.h"
#include "artm/core/merger.h"

namespace artm {
namespace core {

NodeControllerServiceImpl::NodeControllerServiceImpl() : lock_(), instance_(nullptr) { ; }
NodeControllerServiceImpl::~NodeControllerServiceImpl() {}

void NodeControllerServiceImpl::CreateOrReconfigureInstance(
    const ::artm::MasterComponentConfig& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (instance_ != nullptr) {
        instance_->Reconfigure(request);
    } else {
      instance_.reset(new Instance(request, NodeControllerInstance));
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
  instance_.reset();
  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureModel(
    const ::artm::core::CreateOrReconfigureModelArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (instance_ == nullptr) {
      LOG(ERROR) << "Instance not found";
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance_->CreateOrReconfigureModel(request.config());
    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeModel(
    const ::artm::core::DisposeModelArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  if (instance_ != nullptr) {
    instance_->DisposeModel(request.model_name());
  }

  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureRegularizer(
    const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (instance_ == nullptr) {
      LOG(ERROR) << "Instance not found";
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance_->CreateOrReconfigureRegularizer(request.config());
    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeRegularizer(
    const ::artm::core::DisposeRegularizerArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  if (instance_ != nullptr) {
    instance_->DisposeRegularizer(request.regularizer_name());
  }

  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureDictionary(
    const ::artm::core::CreateOrReconfigureDictionaryArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (instance_ == nullptr) {
      LOG(ERROR) << "Instance not found";
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("Instance not found"));
    }

    instance_->CreateOrReconfigureDictionary(request.dictionary());
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
    if (instance_ != nullptr) {
      instance_->DisposeDictionary(request.dictionary_name());
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
    if (instance_ != nullptr) {
      instance_->merger()->ForcePullTopicModel();
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
    if (instance_ != nullptr) {
      instance_->merger()->ForcePushTopicModelIncrement();
    } else {
      LOG(ERROR) << "No instances exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

Instance* NodeControllerServiceImpl::instance() {
  return instance_.get();
}

}  // namespace core
}  // namespace artm
