#ifndef ARTM_INSTANCE_
#define ARTM_INSTANCE_

#include <map>
#include <memory>
#include <queue>
#include <vector>
 
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "artm/common.h"
#include "artm/instance_schema.h"
#include "artm/internals.pb.h"
#include "artm/merger.h"
#include "artm/messages.pb.h"
#include "artm/processor.h"
#include "artm/template_manager.h"
#include "artm/thread_safe_holder.h"

namespace artm {
namespace core {

  class Instance : boost::noncopyable {
  public:
    ~Instance();

    int id() const {
      return instance_id_;
    }

    const std::shared_ptr<InstanceSchema> schema() const {
      return schema_.get();
    }

    int processor_queue_size() const;

    int CreateModel(const ModelConfig& config);
    int ReconfigureModel(int model_id, const ModelConfig& config);
    int DisposeModel(int model_id);
    int Reconfigure(const InstanceConfig& config);
    int RequestModelTopics(int model_id, ModelTopics* model_topics);
    int AddBatchIntoProcessorQueue(std::shared_ptr<const ProcessorInput> input);

  private:
    friend class TemplateManager<Instance, InstanceConfig>;

    // All instances must be created via TemplateManager.
    Instance(int id, const InstanceConfig& config);

    mutable boost::mutex lock_;
    int instance_id_;
    ThreadSafeHolder<InstanceSchema> schema_;

    int next_model_id_;

    mutable boost::mutex processor_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorInput> > processor_queue_;

    mutable boost::mutex merger_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorOutput> > merger_queue_;

    // creates a background thread that keep merging processor output
    Merger merger_;

    // creates background threads for processing
    std::vector<std::shared_ptr<Processor> > processors_;    
  };

  typedef TemplateManager<Instance, InstanceConfig> InstanceManager;


}} // namespace artm/core
#endif // ARTM_INSTANCE_
