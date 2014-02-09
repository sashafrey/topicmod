#include "artm/cpp_interface.h"

namespace artm { 

inline char* string_as_array(string* str) {
  return str->empty() ? NULL : &*str->begin();
}

Instance::Instance(const InstanceConfig& config) : id_(0), config_(config)
{
  string instance_config_blob;
  config.SerializeToString(&instance_config_blob);
  id_ = create_instance(
    0,
    instance_config_blob.size(), 
    string_as_array(&instance_config_blob));
}

Instance::~Instance() {
  dispose_instance(id());
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

  string model_topics_blob;
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

void Instance::WaitModelProcessed(const Model& model, int nDocs) {
  wait_model_processed(id(), model.model_id(), nDocs);
}

Model::Model(const Instance& instance, const ModelConfig& config) : instance_id_(instance.id()), model_id_(0), config_(config) 
{
  string model_config_blob;
    config.SerializeToString(&model_config_blob);
    model_id_ = create_model(
      instance_id_, 
      0,
      model_config_blob.size(), 
      string_as_array(&model_config_blob));
}

Model::~Model() {
  dispose_model(instance_id(), model_id());
}

void Model::Reconfigure(const ModelConfig& config) {
  string model_config_blob;
  config.SerializeToString(&model_config_blob);
  reconfigure_model(instance_id(), model_id(), model_config_blob.size(), string_as_array(&model_config_blob));
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
 
DataLoader::DataLoader(const Instance& instance, const DataLoaderConfig& config) : id_(0), config_(config)
{
  config_.set_instance_id(instance.id());
  string data_loader_config_blob;
  config_.SerializeToString(&data_loader_config_blob);
  id_ =
    create_data_loader(
      0,
      data_loader_config_blob.size(),
      string_as_array(&data_loader_config_blob));      
}

DataLoader::~DataLoader() {
  dispose_data_loader(id());
}
   
void DataLoader::AddBatch(const Batch& batch) {
  string batch_blob;
  batch.SerializeToString(&batch_blob);
  add_batch(id(), batch_blob.size(), string_as_array(&batch_blob));
}
 
} // namespace artm
