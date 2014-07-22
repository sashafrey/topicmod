// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/master_proxy.h"

#include "rpcz/application.hpp"

#include "artm/core/zmq_context.h"


namespace artm {
namespace core {

MasterProxy::MasterProxy(int id, const MasterProxyConfig& config) : id_(id) {
  rpcz::application::options options(3);
  options.zeromq_context = ZmqContext::singleton().get();
  application_.reset(new rpcz::application(options));

  timeout_ = config.timeout();
  node_controller_service_proxy_.reset(
    new artm::core::NodeControllerService_Stub(
      application_->create_rpc_channel(config.node_connect_endpoint()), true));

  make_rpcz_call([&]() {
    Void response;
    node_controller_service_proxy_->CreateOrReconfigureMasterComponent(
      config.config(), &response, timeout_);
  });
}
MasterProxy::~MasterProxy() {
  make_rpcz_call_no_throw([&]() {
    Void response;
    node_controller_service_proxy_->DisposeMasterComponent(Void(), &response, timeout_);
  });
}

int MasterProxy::id() const { return id_; }

void MasterProxy::Reconfigure(const MasterComponentConfig& config) {
  make_rpcz_call([&]() {
    Void response;
    node_controller_service_proxy_->CreateOrReconfigureMasterComponent(
      config, &response, timeout_);
  });
}

void MasterProxy::CreateOrReconfigureModel(const ModelConfig& config) {
  make_rpcz_call([&]() {
    CreateOrReconfigureModelArgs request;
    request.set_model_name(config.name());
    request.mutable_config()->CopyFrom(config);
    Void response;
    node_controller_service_proxy_->CreateOrReconfigureModel(request, &response, timeout_);
  });
}

void MasterProxy::DisposeModel(ModelName model_name) {
  make_rpcz_call_no_throw([&]() {
    DisposeModelArgs args;
    args.set_model_name(model_name);
    Void response;
    node_controller_service_proxy_->DisposeModel(args, &response, timeout_);
  });
}

void MasterProxy::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {
  make_rpcz_call([&]() {
    CreateOrReconfigureRegularizerArgs request;
    request.set_regularizer_name(config.name());
    request.mutable_config()->CopyFrom(config);
    Void response;
    node_controller_service_proxy_->CreateOrReconfigureRegularizer(request, &response, timeout_);
  });
}

void MasterProxy::DisposeRegularizer(const std::string& name) {
  make_rpcz_call_no_throw([&]() {
    DisposeRegularizerArgs args;
    args.set_regularizer_name(name);
    Void response;
    node_controller_service_proxy_->DisposeRegularizer(args, &response, timeout_);
  });
}

void MasterProxy::CreateOrReconfigureDictionary(const DictionaryConfig& config) {
  make_rpcz_call([&]() {
    CreateOrReconfigureDictionaryArgs request;
    request.mutable_dictionary()->CopyFrom(config);
    Void response;
    node_controller_service_proxy_->CreateOrReconfigureDictionary(request, &response, timeout_);
  });
}

void MasterProxy::DisposeDictionary(const std::string& name) {
  make_rpcz_call_no_throw([&]() {
    DisposeDictionaryArgs args;
    args.set_dictionary_name(name);
    Void response;
    node_controller_service_proxy_->DisposeDictionary(args, &response, timeout_);
  });
}

void MasterProxy::OverwriteTopicModel(const ::artm::TopicModel& topic_model) {
  make_rpcz_call([&]() {
    Void response;
    node_controller_service_proxy_->OverwriteTopicModel(topic_model, &response, timeout_);
  });
}

bool MasterProxy::RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) {
  make_rpcz_call([&]() {
    String request;
    request.set_value(model_name);
    node_controller_service_proxy_->RequestTopicModel(request, topic_model, timeout_);
  });

  return true;
}

bool MasterProxy::RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix) {
  make_rpcz_call([&]() {
    String request;
    request.set_value(model_name);
    node_controller_service_proxy_->RequestThetaMatrix(request, theta_matrix, timeout_);
  });

  return true;
}

bool MasterProxy::RequestScore(const ModelName& model_name, const ScoreName& score_name,
                               ::artm::ScoreData* score_data) {
  make_rpcz_call([&]() {
    RequestScoreArgs request;
    request.set_model_name(model_name);
    request.set_score_name(score_name);
    node_controller_service_proxy_->RequestScore(request, score_data, timeout_);
  });

  return true;
}

void MasterProxy::AddBatch(const Batch& batch) {
  make_rpcz_call([&]() {
    Void response;
    node_controller_service_proxy_->AddBatch(batch, &response, timeout_);
  });
}

void MasterProxy::InvokeIteration(int iterations_count) {
  make_rpcz_call([&]() {
    Void response;
    node_controller_service_proxy_->InvokeIteration(Void(), &response, timeout_);
  });
}

void MasterProxy::WaitIdle() {
  make_rpcz_call([&]() {
    Void response;
    node_controller_service_proxy_->WaitIdle(Void(), &response, timeout_);
  });
}

void MasterProxy::InvokePhiRegularizers() {
  make_rpcz_call([&]() {
    Void response;
    node_controller_service_proxy_->InvokePhiRegularizers(Void(), &response, timeout_);
  });
}

}  // namespace core
}  // namespace artm

