// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CPP_INTERFACE_H_
#define SRC_ARTM_CPP_INTERFACE_H_

#include <memory>
#include <string>

#include "artm/messages.pb.h"
#include "artm/c_interface.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);   \
  void operator=(const TypeName&)

namespace artm {

class Instance;
class Model;
class Regularizer;
class DataLoader;

// Exception handling in cpp_interface
#define DEFINE_EXCEPTION_TYPE(Type, BaseType)          \
class Type : public BaseType { public:  /*NOLINT*/     \
  explicit Type() : BaseType("") {}                    \
};

DEFINE_EXCEPTION_TYPE(GeneralError, std::runtime_error);
DEFINE_EXCEPTION_TYPE(ObjectNotFound, std::runtime_error);
DEFINE_EXCEPTION_TYPE(InvalidMessage, std::runtime_error);
DEFINE_EXCEPTION_TYPE(UnsupportedReconfiguration, std::runtime_error);

#undef DEFINE_EXCEPTION_TYPE

class Instance {
 public:
  explicit Instance(const InstanceConfig& config);
  ~Instance();

  int id() const { return id_; }
  std::shared_ptr<TopicModel> GetTopicModel(const Model& model);
  void Reconfigure(const InstanceConfig& config);

  const InstanceConfig& config() const { return config_; }

 private:
  int id_;
  InstanceConfig config_;
  DISALLOW_COPY_AND_ASSIGN(Instance);
};

class Model {
 public:
  Model(const Instance& instance, const ModelConfig& config);
  ~Model();

  void Reconfigure(const ModelConfig& config);
  void Enable();
  void Disable();
  void InvokePhiRegularizers();

  int instance_id() const { return instance_id_; }
  const std::string& model_id() const { return config_.model_id(); }

  const ModelConfig& config() const { return config_; }

 private:
  int instance_id_;
  ModelConfig config_;
  DISALLOW_COPY_AND_ASSIGN(Model);
};

class Regularizer {
 public:
  Regularizer(const Instance& instance, const RegularizerConfig& config);
  ~Regularizer();

  void Reconfigure(const RegularizerConfig& config);

  int instance_id() const { return instance_id_; }
  const RegularizerConfig& config() const { return config_; }

 private:
  int instance_id_;
  RegularizerConfig config_;
  DISALLOW_COPY_AND_ASSIGN(Regularizer);
};

class DataLoader {
 public:
  DataLoader(const Instance& instance, const DataLoaderConfig& config);
  ~DataLoader();

  int id() const { return id_; }
  void AddBatch(const Batch& batch);
  void AddStream(const Stream& stream);
  void RemoveStream(std::string stream_name);
  void Reconfigure(const DataLoaderConfig& config);
  void InvokeIteration(int iterations_count);
  void WaitIdle();
  const DataLoaderConfig& config() const { return config_; }

 private:
  int id_;
  DataLoaderConfig config_;
  DISALLOW_COPY_AND_ASSIGN(DataLoader);
};

class MemcachedServer {
 public:
  explicit MemcachedServer(const std::string& endpoint);
  ~MemcachedServer();

 private:
  int id_;
};

}  // namespace artm

#endif  // SRC_ARTM_CPP_INTERFACE_H_
