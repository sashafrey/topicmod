#ifndef TOPICMD_INSTANCE_
#define TOPICMD_INSTANCE_

#include <map>
#include <memory>
#include <vector>

#include "messages.pb.h"

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
    const std::map<int, Partition::Ptr>& get() const {
      return generation_;
    }
    
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

  class GenerationManager {
  private:
    // Implements read-only pattern for thread safety
    Generation::Ptr published_generation_;

  public:
    GenerationManager() 
      : published_generation_(new Generation())
    {
    }

    void AddPartition(int id, Partition::Ptr partition) {
      // make a copy of published_generation_, 
      // and then set it in atomic way.
      Generation::Ptr next_generation_(new Generation());
      auto current_generation_ = GetLatestGeneration();
      for (auto iter = current_generation_->get().begin();
	   iter != current_generation_->get().end();
	   ++iter) {
	next_generation_->mutable_get().insert(*iter);
      }

      next_generation_->mutable_get()
	.insert(std::pair<int, Partition::Ptr>(id, 
					       partition));
      
      // lock
      published_generation_ = next_generation_;
    }

    // Users of this function must hold shared_ptr while they 
    // are using the generation.
    Generation::Ptr GetLatestGeneration() const {
      // lock
      return published_generation_;
    }
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
    int instance_id_;
    int next_generation_id_;
    Partition::Ptr current_partition_;
    std::map<int, Partition::Ptr> finished_partition_;
    GenerationManager generation_manager_;

    std::shared_ptr<InstanceConfig> instance_config_;
  };

} // namespace topicmd
#endif // TOPICMD_INSTANCE_
