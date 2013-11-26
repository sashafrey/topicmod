#ifndef TOPICMD_INSTANCE_
#define TOPICMD_INSTANCE_

#include <map>
#include <memory>
#include <vector>

#include <boost/thread/mutex.hpp>

#include "topicmd/messages.pb.h"
#include "topicmd/thread_safe_holder.h"

namespace topicmd {

  class Partition {
  private: 
    std::vector<std::shared_ptr<Batch> > batches_;
  public:
    Partition() { };
    void Add(const Batch& batch) {
      batches_.push_back(std::shared_ptr<Batch>(new Batch(batch)));
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

    typedef std::shared_ptr<Partition> Ptr;
  };

  class Generation {
  private:
    std::map<int, Partition::Ptr> generation_;
  public:
    void AddPartition(int id, const Partition::Ptr& partition) {
      generation_.insert(std::make_pair(id, partition));
    }

    const std::map<int, Partition::Ptr>& get() const {
      return generation_;
    }

    // ToDo : code style: agree on consistency (mutable_get() vs get())    
    std::map<int, Partition::Ptr>& mutable_get() {
      return generation_;
    }

    int GetTotalItemsCount() const {
      int retval = 0;
      for (auto iter = get().begin();
	   iter != get().end();
	   ++iter) {
	retval += (*iter).second->GetItemsCount();
      }

      return retval;
    }

    typedef std::shared_ptr<Generation> Ptr;
  };

  class Instance {
  public:
    typedef std::shared_ptr<Instance> Ptr;
    Instance(int id, const InstanceConfig& config);
    int id() const {
      return instance_id_;
    }

    const std::shared_ptr<InstanceConfig> config() const {
      return instance_config_;
    }

    int FinishPartition();
    int GetTotalItemsCount() const;
    int InsertBatch(const Batch& batch);
    int PublishGeneration(int generation_id);
    
  private:
    mutable boost::mutex lock_;
    int instance_id_;
    int next_generation_id_;
    Partition::Ptr current_partition_;
    std::map<int, Partition::Ptr> finished_partition_;
    ThreadSafeHolder<Generation> published_generation_;
    std::shared_ptr<InstanceConfig> instance_config_;
  };

} // namespace topicmd
#endif // TOPICMD_INSTANCE_
