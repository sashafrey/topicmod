// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/master_proxy.h"

namespace artm {
namespace core {

MasterProxy::MasterProxy(int id, const MasterProxyConfig& config) : id_(id) {}
MasterProxy::~MasterProxy() {}

int MasterProxy::id() const { return id_; }

void MasterProxy::Reconfigure(const MasterComponentConfig& config) {}

void MasterProxy::CreateOrReconfigureModel(const ModelConfig& config) {}
void MasterProxy::DisposeModel(ModelName model_name) {}

void MasterProxy::CreateOrReconfigureRegularizer(const RegularizerConfig& config) {}
void MasterProxy::DisposeRegularizer(const std::string& name) {}

void MasterProxy::CreateOrReconfigureDictionary(const DictionaryConfig& config) {}
void MasterProxy::DisposeDictionary(const std::string& name) {}

void MasterProxy::OverwriteTopicModel(const ::artm::TopicModel& topic_model) {}
bool MasterProxy::RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model) { return true; }
bool MasterProxy::RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix) { return true; }

void MasterProxy::AddBatch(const Batch& batch) {}
void MasterProxy::InvokeIteration(int iterations_count) {}
void MasterProxy::WaitIdle() {}
void MasterProxy::InvokePhiRegularizers() {}

}  // namespace core
}  // namespace artm

