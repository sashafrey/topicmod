// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/cpp_interface.h"
#include "artm/protobuf_helpers.h"

namespace artm {

inline char* StringAsArray(std::string* str) {
  return str->empty() ? NULL : &*str->begin();
}

inline int HandleErrorCode(int artm_error_code) {
  // All error codes are negative. Any non-negative value is a success.
  if (artm_error_code >= 0) {
    return artm_error_code;
  }

  switch (artm_error_code) {
    case ARTM_SUCCESS:
      return artm_error_code;
    case ARTM_OBJECT_NOT_FOUND:
      throw ObjectNotFound();
    case ARTM_INVALID_MESSAGE:
      throw InvalidMessage();
    case ARTM_UNSUPPORTED_RECONFIGURATION:
      throw UnsupportedReconfiguration();
    case ARTM_GENERAL_ERROR:
    default:
      throw GeneralError();
  }
}

Instance::Instance(const InstanceConfig& config) : id_(0), config_(config) {
  std::string instance_config_blob;
  config.SerializeToString(&instance_config_blob);
  id_ = HandleErrorCode(ArtmCreateInstance(0, instance_config_blob.size(),
    StringAsArray(&instance_config_blob)));
}

Instance::~Instance() {
  ArtmDisposeInstance(id());
}

void Instance::Reconfigure(const InstanceConfig& config) {
  std::string config_blob;
  config.SerializeToString(&config_blob);
  HandleErrorCode(ArtmReconfigureInstance(id(), config_blob.size(), StringAsArray(&config_blob)));
  config_.CopyFrom(config);
}

std::shared_ptr<ModelTopics> Instance::GetTopics(const Model& model) {
  // Request model topics
  int request_id = HandleErrorCode(ArtmRequestModelTopics(
    id(), model.model_id()));

  int length = HandleErrorCode(ArtmGetRequestLength(request_id));
  std::string model_topics_blob;
  model_topics_blob.resize(length);
  HandleErrorCode(ArtmCopyRequestResult(request_id, length, StringAsArray(&model_topics_blob)));

  ArtmDisposeRequest(request_id);

  std::shared_ptr<ModelTopics> model_topics(new ModelTopics());
  model_topics->ParseFromString(model_topics_blob);
  return model_topics;
}

Model::Model(const Instance& instance, const ModelConfig& config)
    : instance_id_(instance.id()),
      model_id_(0),
      config_(config) {
  std::string model_config_blob;
  config.SerializeToString(&model_config_blob);
  model_id_ = HandleErrorCode(ArtmCreateModel(instance_id_, model_config_blob.size(),
    StringAsArray(&model_config_blob)));
}

Model::~Model() {
  ArtmDisposeModel(instance_id(), model_id());
}

void Model::Reconfigure(const ModelConfig& config) {
  std::string model_config_blob;
  config.SerializeToString(&model_config_blob);
  HandleErrorCode(ArtmReconfigureModel(instance_id(), model_id(), model_config_blob.size(),
    StringAsArray(&model_config_blob)));
  config_.CopyFrom(config);
}

void Model::Enable() {
  ModelConfig config_copy_(config_);
  config_copy_.set_enabled(true);
  Reconfigure(config_copy_);
}

void Model::Disable() {
  ModelConfig config_copy_(config_);
  config_copy_.set_enabled(false);
  Reconfigure(config_copy_);
}

DataLoader::DataLoader(const Instance& instance, const DataLoaderConfig& config)
    : id_(0), config_(config) {
  config_.set_instance_id(instance.id());
  std::string data_loader_config_blob;
  config_.SerializeToString(&data_loader_config_blob);
  id_ = HandleErrorCode(ArtmCreateDataLoader(0, data_loader_config_blob.size(),
    StringAsArray(&data_loader_config_blob)));
}

DataLoader::~DataLoader() {
  ArtmDisposeDataLoader(id());
}

void DataLoader::AddBatch(const Batch& batch) {
  std::string batch_blob;
  batch.SerializeToString(&batch_blob);
  HandleErrorCode(ArtmAddBatch(id(), batch_blob.size(), StringAsArray(&batch_blob)));
}

void DataLoader::Reconfigure(const DataLoaderConfig& config) {
  std::string config_blob;
  config.SerializeToString(&config_blob);
  HandleErrorCode(ArtmReconfigureDataLoader(id(), config_blob.size(), StringAsArray(&config_blob)));
  config_.CopyFrom(config);
}

void DataLoader::InvokeIteration(int iterations_count) {
  HandleErrorCode(ArtmInvokeIteration(id(), iterations_count));
}

void DataLoader::WaitIdle() {
  HandleErrorCode(ArtmWaitIdleDataLoader(id()));
}

void DataLoader::AddStream(const Stream& stream) {
  Stream* s = config_.add_stream();
  s->CopyFrom(stream);
  Reconfigure(config_);
}

void DataLoader::RemoveStream(std::string stream_name) {
  DataLoaderConfig new_config(config_);
  new_config.mutable_stream()->Clear();

  for (int stream_index = 0; stream_index < config_.stream_size(); ++stream_index) {
    if (config_.stream(stream_index).name() != stream_name) {
      Stream* s = new_config.add_stream();
      s->CopyFrom(config_.stream(stream_index));
    }
  }

  Reconfigure(new_config);
}

MemcachedServer::MemcachedServer(const std::string& endpoint) {
  id_ = HandleErrorCode(ArtmCreateMemcachedServer(endpoint.c_str()));
}

MemcachedServer::~MemcachedServer() {
  ArtmDisposeMemcachedServer(id_);
}

}  // namespace artm
