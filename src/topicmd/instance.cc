#include "topicmd/common.h"
#include "topicmd/instance.h"

namespace topicmd {
  Instance::Instance(int id, const InstanceConfig& config) : 
    lock_(),
    instance_id_(id),
    next_generation_id_(1),
    current_partition_(Partition::Ptr(new Partition())),
    finished_partition_(),
    published_generation_(lock_),
    instance_config_(new InstanceConfig(config))
  {
  }

  int Instance::GetTotalItemsCount() const {
    auto latest = published_generation_.get();
    return latest->GetTotalItemsCount();
  }

  int Instance::FinishPartition() {
    int last_id = next_generation_id_++;
    finished_partition_
      .insert(std::pair<int, Partition::Ptr>(last_id, 
					     current_partition_));

    current_partition_ = Partition::Ptr(new Partition());
    return last_id;
  }

  int Instance::InsertBatch(const Batch& batch) {
    current_partition_->Add(batch);
    return TOPICMD_SUCCESS;
  }

  int Instance::PublishGeneration(int generation_id) 
  {
    // Don't lock the whole thing. For now the topicmd library
    // assumes that caller is single-threaded.
    auto next_generation_ = published_generation_.get_copy();

    // move all generations with id <= generation_id
    // from finished_generation_ to published_generations_.
    for (auto iter = finished_partition_.begin();
	 iter != finished_partition_.end(); 
	 /* nothing */) 
    {
      if ((*iter).first <= generation_id) {
	next_generation_->AddPartition((*iter).first, iter->second);
	finished_partition_.erase(iter++);
      }
      else {
	++iter;
      }
    }

    published_generation_.set(next_generation_);
    return TOPICMD_SUCCESS;
  }


} // namespace topicmd
