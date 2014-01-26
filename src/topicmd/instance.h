#ifndef TOPICMD_INSTANCE_
#define TOPICMD_INSTANCE_

#include <map>
#include <memory>
#include <queue>
#include <vector>
  
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/data_loader.h"
#include "topicmd/instance_schema.h"
#include "topicmd/merger.h"
#include "topicmd/messages.pb.h"
#include "topicmd/partition.h"
#include "topicmd/processor.h"
#include "topicmd/thread_safe_holder.h"

namespace topicmd {
  class Instance : boost::noncopyable {
  public:
    Instance(int id, const InstanceConfig& config);
    ~Instance();
    int id() const {
      return instance_id_;
    }

    const std::shared_ptr<InstanceSchema> schema() const {
      return schema_.get();
    }

    int UpdateModel(int model_id, const ModelConfig& config);
    int DisposeModel(int model_id);
    int DiscardPartition();
    int FinishPartition();
    int GetTotalItemsCount() const;
    int InsertBatch(const Batch& batch);
    int PublishGeneration(int generation_id);
    int Reconfigure(const InstanceConfig& config);
    int RequestModelTopics(int model_id, ModelTopics* model_topics);
    int WaitModelProcessed(int model_id, int processed_items);

    std::shared_ptr<const Generation> get_latest_generation() const {
      return published_generation_.get();
    }
  private:
    mutable boost::mutex lock_;
    int instance_id_;
    int next_generation_id_;
    std::shared_ptr<Partition> current_partition_;
    std::map<int, std::shared_ptr<const Partition> > finished_partition_;
    ThreadSafeHolder<Generation> published_generation_;
    ThreadSafeHolder<InstanceSchema> schema_;

    // ToDo: processor queue must have not parts, 
    // but special processing batches, self-containing data structures
    // to process them in a processor.
    mutable boost::mutex processor_queue_lock_;
    std::queue<std::shared_ptr<const Partition> > processor_queue_;

    mutable boost::mutex merger_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorOutput> > merger_queue_;

    DataLoader data_loader_; // creates a background thread that keep loading data
    Merger merger_;           // creates a background thread that keep merging processor output
    std::vector<std::shared_ptr<Processor> > processors_;     // creates a background thread for processing (for now only one thread)
  };

} // namespace topicmd
#endif // TOPICMD_INSTANCE_
