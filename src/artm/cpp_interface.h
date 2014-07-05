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

class MasterComponent;
class NodeController;
class Model;
class Regularizer;
class Dictionary;

// Exception handling in cpp_interface
#define DEFINE_EXCEPTION_TYPE(Type, BaseType)          \
class Type : public BaseType { public:  /*NOLINT*/     \
  explicit Type() : BaseType("") {}                    \
};

DEFINE_EXCEPTION_TYPE(GeneralError, std::runtime_error);
DEFINE_EXCEPTION_TYPE(ObjectNotFound, std::runtime_error);
DEFINE_EXCEPTION_TYPE(InvalidMessage, std::runtime_error);
DEFINE_EXCEPTION_TYPE(InvalidOperation, std::runtime_error);

#undef DEFINE_EXCEPTION_TYPE

class MasterComponent {
 public:
  explicit MasterComponent(const MasterComponentConfig& config);
  explicit MasterComponent(const MasterProxyConfig& config);
  ~MasterComponent();

  int id() const { return id_; }
  std::shared_ptr<TopicModel> GetTopicModel(const Model& model);
  std::shared_ptr<ThetaMatrix> GetThetaMatrix(const Model& model);
  void Reconfigure(const MasterComponentConfig& config);
  void AddBatch(const Batch& batch);
  void AddStream(const Stream& stream);
  void RemoveStream(std::string stream_name);
  void InvokeIteration(int iterations_count);
  void WaitIdle();

  const MasterComponentConfig& config() const { return config_; }
  MasterComponentConfig* mutable_config() { return &config_; }

 private:
  int id_;
  MasterComponentConfig config_;
  DISALLOW_COPY_AND_ASSIGN(MasterComponent);
};

class NodeController {
 public:
  explicit NodeController(const NodeControllerConfig& config);
  ~NodeController();

  int id() const { return id_; }

 private:
  int id_;
  NodeControllerConfig config_;
  DISALLOW_COPY_AND_ASSIGN(NodeController);
};

class Model {
 public:
  Model(const MasterComponent& master_component, const ModelConfig& config);
  ~Model();

  void Reconfigure(const ModelConfig& config);
  void Overwrite(const TopicModel& topic_model);
  void Enable();
  void Disable();
  void InvokePhiRegularizers();

  int master_id() const { return master_id_; }
  const std::string& name() const { return config_.name(); }

  const ModelConfig& config() const { return config_; }
  ModelConfig* mutable_config() { return &config_; }
 private:
  int master_id_;
  ModelConfig config_;
  DISALLOW_COPY_AND_ASSIGN(Model);
};

class Regularizer {
 public:
  Regularizer(const MasterComponent& master_component, const RegularizerConfig& config);
  ~Regularizer();

  void Reconfigure(const RegularizerConfig& config);

  int master_id() const { return master_id_; }
  const RegularizerConfig& config() const { return config_; }
  RegularizerConfig* mutable_config() { return &config_; }

 private:
  int master_id_;
  RegularizerConfig config_;
  DISALLOW_COPY_AND_ASSIGN(Regularizer);
};

class Dictionary {
 public:
  Dictionary(const MasterComponent& master_component, const DictionaryConfig& config);
  ~Dictionary();

  void Reconfigure(const DictionaryConfig& config);

  int master_id() const { return master_id_; }
  const DictionaryConfig& config() const { return config_; }

 private:
  int master_id_;
  DictionaryConfig config_;
  DISALLOW_COPY_AND_ASSIGN(Dictionary);
};

}  // namespace artm

#endif  // SRC_ARTM_CPP_INTERFACE_H_
