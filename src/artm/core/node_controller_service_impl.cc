// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/node_controller_service_impl.h"

#include <string>

#include "boost/thread.hpp"

#include "glog/logging.h"

#include "artm/core/instance.h"
#include "artm/core/master_component.h"
#include "artm/core/exceptions.h"
#include "artm/core/merger.h"

namespace artm {
namespace core {

NodeControllerServiceImpl::NodeControllerServiceImpl()
    : lock_(), instance_(nullptr), master_(nullptr) { }

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


void NodeControllerServiceImpl::CreateOrReconfigureMasterComponent(
    const ::artm::MasterComponentConfig& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (master_ != nullptr) {
      master_->Reconfigure(request);
    } else {
      auto& mcm = artm::core::MasterComponentManager::singleton();
      int master_id = mcm.Create<MasterComponent, MasterComponentConfig>(request);
      assert(master_id > 0);
      master_ = mcm.Get(master_id);
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::DisposeMasterComponent(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  boost::lock_guard<boost::mutex> guard(lock_);
  if (master_ != nullptr) {
    auto& mcm = artm::core::MasterComponentManager::singleton();
    mcm.Erase(master_->id());
    master_.reset();
  }

  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureModel(
    const ::artm::core::CreateOrReconfigureModelArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    VerifyCurrentState();

    if (instance_ != nullptr) {
      instance_->CreateOrReconfigureModel(request.config());
    }

    if (master_ != nullptr) {
      master_->CreateOrReconfigureModel(request.config());
    }

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

  if (master_ != nullptr) {
    master_->DisposeModel(request.model_name());
  }

  response.send(Void());
}

void NodeControllerServiceImpl::CreateOrReconfigureRegularizer(
    const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    VerifyCurrentState();

    if (instance_ != nullptr) {
      instance_->CreateOrReconfigureRegularizer(request.config());
    }

    if (master_ != nullptr) {
      master_->CreateOrReconfigureRegularizer(request.config());
    }

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
    VerifyCurrentState();

    if (instance_ != nullptr) {
      instance_->CreateOrReconfigureDictionary(request.dictionary());
    }

    if (master_ != nullptr) {
      master_->CreateOrReconfigureDictionary(request.dictionary());
    }

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

void NodeControllerServiceImpl::OverwriteTopicModel(
    const ::artm::TopicModel& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (master_ != nullptr) {
      master_->OverwriteTopicModel(request);
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::RequestTopicModel(
    const ::artm::core::String& request,
    ::rpcz::reply< ::artm::TopicModel> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    artm::TopicModel topic_model;
    bool ok = false;
    if (master_ != nullptr) {
      ok = master_->RequestTopicModel(request.value(), &topic_model);
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    if (ok) {
      response.send(topic_model);
    } else {
      response.Error(-1);  // todo(alfrey): fix error handling in services
    }
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::RequestThetaMatrix(
    const ::artm::core::String& request,
    ::rpcz::reply< ::artm::ThetaMatrix> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    artm::ThetaMatrix theta_matrix;
    bool ok = false;
    if (master_ != nullptr) {
      ok = master_->RequestThetaMatrix(request.value(), &theta_matrix);
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    if (ok) {
      response.send(theta_matrix);
    } else {
      response.Error(-1);  // todo(alfrey): fix error handling in services
    }
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::RequestScore(
    const ::artm::core::RequestScoreArgs& request,
    ::rpcz::reply< ::artm::ScoreData> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    artm::ScoreData score_data;
    bool ok = false;
    if (master_ != nullptr) {
      ok = master_->RequestScore(request.model_name(), request.score_name(), &score_data);
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    if (ok) {
      response.send(score_data);
    } else {
      response.Error(-1);  // todo(alfrey): fix error handling in services
    }
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::AddBatch(
    const ::artm::Batch& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (master_ != nullptr) {
      master_->AddBatch(request);
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::InvokeIteration(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (master_ != nullptr) {
      master_->InvokeIteration(1);
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}
void NodeControllerServiceImpl::WaitIdle(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (master_ != nullptr) {
      master_->WaitIdle();
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

void NodeControllerServiceImpl::InvokePhiRegularizers(
    const ::artm::core::Void& request,
    ::rpcz::reply< ::artm::core::Void> response) {
  try {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (master_ != nullptr) {
      master_->InvokePhiRegularizers();
    } else {
      LOG(ERROR) << "No master component exist in node controller";
    }

    response.send(Void());
  } catch(...) {
    response.Error(-1);  // todo(alfrey): fix error handling in services
  }
}

Instance* NodeControllerServiceImpl::instance() {
  return instance_.get();
}

void NodeControllerServiceImpl::VerifyCurrentState() {
  if ((instance_ == nullptr) && (master_ == nullptr)) {
    std::string message = "Neither Instance nor MasterComponent had been found";
    LOG(ERROR) << message;
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(message));
  }

  if ((instance_ != nullptr) && (master_ != nullptr)) {
    std::string message = "Instance and MasterComponent exist together in on node controller";
    LOG(ERROR) << message;
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(message));
  }
}

}  // namespace core
}  // namespace artm
