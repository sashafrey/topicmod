#ifndef TOPICMD_INSTANCE_
#define TOPICMD_INSTANCE_

#include <map>
#include <memory>
#include <queue>
#include <vector>
  
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/instance_schema.h"
#include "topicmd/internals.pb.h"
#include "topicmd/merger.h"
#include "topicmd/messages.pb.h"
#include "topicmd/processor.h"
#include "topicmd/template_manager.h"
#include "topicmd/thread_safe_holder.h"

namespace topicmd {
  class Instance : boost::noncopyable {
  public:
    ~Instance();

    int id() const {
      return instance_id_;
    }

    const std::shared_ptr<InstanceSchema> schema() const {
      return schema_.get();
    }

    int UpdateModel(int model_id, const ModelConfig& config);
    int DisposeModel(int model_id);
    int Reconfigure(const InstanceConfig& config);
    int RequestModelTopics(int model_id, ModelTopics* model_topics);
    int WaitModelProcessed(int model_id, int processed_items);
    int ProcessorQueueSize();
    int AddBatchIntoProcessorQueue(std::shared_ptr<const Batch> batch);

  private:
    friend class TemplateManager<Instance, InstanceConfig>;

    // All instances must be created via TemplateManager.
    Instance(int id, const InstanceConfig& config);

    mutable boost::mutex lock_;
    int instance_id_;
    ThreadSafeHolder<InstanceSchema> schema_;

    mutable boost::mutex processor_queue_lock_;
    std::queue<std::shared_ptr<const Batch> > processor_queue_;

    mutable boost::mutex merger_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorOutput> > merger_queue_;

    // creates a background thread that keep merging processor output
    Merger merger_; 

    // creates background threads for processing
    std::vector<std::shared_ptr<Processor> > processors_;     
  };

  typedef TemplateManager<Instance, InstanceConfig> InstanceManager;


} // namespace topicmd
#endif // TOPICMD_INSTANCE_
