// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/cpp_interface.h"
#include "artm/protobuf_helpers.h"

namespace artm {

inline char* string_as_array(std::string* str) {
  return str->empty() ? NULL : &*str->begin();
}

Instance::Instance(const InstanceConfig& config) : id_(0), config_(config) {
  std::string instance_config_blob;
  config.SerializeToString(&instance_config_blob);
  id_ = create_instance(
    0,
    instance_config_blob.size(),
    string_as_array(&instance_config_blob));
}

Instance::~Instance() {
  dispose_instance(id());
}

void Instance::Reconfigure(const InstanceConfig& config) {
  std::string config_blob;
  config.SerializeToString(&config_blob);
  reconfigure_instance(id(), config_blob.size(), string_as_array(&config_blob));
  config_.CopyFrom(config);
}

std::shared_ptr<ModelTopics> Instance::GetTopics(const Model& model) {
  // Request model topics
  int length;
  char* address;
  int request_id =
    request_model_topics(
      id(),
      model.model_id(),
      &length,
      &address);

  std::string model_topics_blob;
  model_topics_blob.resize(length);
  copy_request_result(
    request_id,
    length,
    string_as_array(&model_topics_blob));

  dispose_request(request_id);

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
  model_id_ = create_model(instance_id_, model_config_blob.size(),
                           string_as_array(&model_config_blob));
}

Model::~Model() {
  dispose_model(instance_id(), model_id());
}

void Model::Reconfigure(const ModelConfig& config) {
  std::string model_config_blob;
  config.SerializeToString(&model_config_blob);
  reconfigure_model(instance_id(), model_id(), model_config_blob.size(),
                    string_as_array(&model_config_blob));
  config_.CopyFrom(config);
}

void Model::Enable() {
  config_.set_enabled(true);
  Reconfigure(config_);
}

void Model::Disable() {
  config_.set_enabled(false);
  Reconfigure(config_);
}

DataLoader::DataLoader(const Instance& instance, const DataLoaderConfig& config)
    : id_(0), config_(config) {
  config_.set_instance_id(instance.id());
  std::string data_loader_config_blob;
  config_.SerializeToString(&data_loader_config_blob);
  id_ = create_data_loader(0, data_loader_config_blob.size(),
                           string_as_array(&data_loader_config_blob));
}

DataLoader::~DataLoader() {
  dispose_data_loader(id());
}

void DataLoader::AddBatch(const Batch& batch) {
  std::string batch_blob;
  batch.SerializeToString(&batch_blob);
  add_batch(id(), batch_blob.size(), string_as_array(&batch_blob));
}

void DataLoader::Reconfigure(const DataLoaderConfig& config) {
  std::string config_blob;
  config.SerializeToString(&config_blob);
  reconfigure_data_loader(id(), config_blob.size(), string_as_array(&config_blob));
  config_.CopyFrom(config);
}

void DataLoader::InvokeIteration(int iterations_count) {
  invoke_iteration(id(), iterations_count);
}

void DataLoader::WaitIdle() {
  wait_idle_data_loader(id());
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

}  // namespace artm
