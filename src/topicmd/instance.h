#ifndef TOPICMD_INSTANCE_
#define TOPICMD_INSTANCE_

#include <map>
#include <memory>
#include <vector>

#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/messages.pb.h"
#include "topicmd/thread_safe_holder.h"

namespace topicmd {

  class Partition : boost::noncopyable {
  private: 
    std::vector<std::shared_ptr<Batch> > batches_;
  public:
    Partition() { };
    void Add(const Batch& batch) {
      batches_.push_back(std::make_shared<Batch>(batch));
    }
    
    int GetItemsCount() const {
      int retval = 0;
      for (auto iter = batches_.begin(); 
	   iter != batches_.end();
	   ++iter) {
	retval += (*iter)->item_size();
      }
      
      return retval;
    }
  };

  class Generation {
  private:
    std::map<int, std::shared_ptr<const Partition> > generation_;
  public:
    void AddPartition(int id,
		      const std::shared_ptr<const Partition>& partition) 
    {
      generation_.insert(std::make_pair(id, partition));
    }

    int GetTotalItemsCount() const {
      int retval = 0;
      for (auto iter = generation_.begin();
	   iter != generation_.end();
	   ++iter) {
	retval += (*iter).second->GetItemsCount();
      }

      return retval;
    }
  };

  class Instance : boost::noncopyable {
  public:
    Instance(int id, const InstanceConfig& config);
    int id() const {
      return instance_id_;
    }

    const std::shared_ptr<InstanceConfig> config() const {
      return instance_config_.get();
    }

    int DiscardPartition();
    int FinishPartition();
    int GetTotalItemsCount() const;
    int InsertBatch(const Batch& batch);
    int PublishGeneration(int generation_id);
    int Reconfigure(const InstanceConfig& config);
    
  private:
    mutable boost::mutex lock_;
    int instance_id_;
    int next_generation_id_;
    std::shared_ptr<Partition> current_partition_;
    std::map<int, std::shared_ptr<const Partition> > finished_partition_;
    ThreadSafeHolder<Generation> published_generation_;
    ThreadSafeHolder<InstanceConfig> instance_config_;
  };

} // namespace topicmd
#endif // TOPICMD_INSTANCE_
