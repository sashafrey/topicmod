#ifndef TOPICMD_CPP_H
#define TOPICMD_CPP_H

#include <memory>
#include <string>
using namespace std;

#include "topicmd/messages.pb.h"
#include "topicmd/c_interface.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);   \
  void operator=(const TypeName&)

namespace artm { 
  class Instance;
  class Model;
  class DataLoader;

  class Instance {
  public:
    explicit Instance(const InstanceConfig& config);
    ~Instance();

    int id() const { return id_; }
    std::shared_ptr<ModelTopics> GetTopics(const Model& model);
    void WaitModelProcessed(const Model& model, int nDocs);
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

    int instance_id() const { return instance_id_; }
    int model_id() const { return model_id_; }

  private:
    int instance_id_;
    int model_id_;
    ModelConfig config_;

    DISALLOW_COPY_AND_ASSIGN(Model);
  };

  class DataLoader {
  public:
    DataLoader(const Instance& instance, const DataLoaderConfig& config);
    ~DataLoader();

    int id() const { return id_; }
    void AddBatch(const Batch& batch);

  private:
    int id_;
    DataLoaderConfig config_;
    DISALLOW_COPY_AND_ASSIGN(DataLoader);
  };
} // namespace artm

#endif // TOPICMD_CPP_H
