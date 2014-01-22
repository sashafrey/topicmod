#ifndef TOPICMD_INSTANCE_
#define TOPICMD_INSTANCE_

#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <vector>
    
#include <boost/utility.hpp>

#include "topicmd/data_loader.h"
#include "topicmd/merger.h"
#include "topicmd/messages.pb.h"
#include "topicmd/partition.h"
#include "topicmd/processor.h"
#include "topicmd/thread_safe_holder.h"

namespace topicmd {
	class InstanceSchema {
	private:
		InstanceConfig instance_config_;
		std::map<int, std::shared_ptr<const ModelConfig> > models_config_;
	public:
		InstanceSchema(const InstanceSchema& schema) :
				instance_config_(schema.instance_config_), 
				models_config_(schema.models_config_) 
		{
		}

		InstanceSchema(const InstanceConfig& config) :
				instance_config_(config), 
				models_config_() 
		{
		}

		void set_instance_config(const InstanceConfig& instance_config) {
			instance_config_.CopyFrom(instance_config);
		}

		const InstanceConfig& get_instance_config() const {
			return instance_config_;
		}

		void set_model_config(int id, const std::shared_ptr<const ModelConfig>& model_config) {
			auto iter = models_config_.find(id);
			if (iter != models_config_.end()) {
				iter->second = model_config;
			} else {
				models_config_.insert(std::make_pair(id, model_config));
			}
		}

		const ModelConfig& get_model_config(int id) {
			auto iter = models_config_.find(id);
			return *(iter->second);
		}

		bool has_model_config(int id) {
			auto iter = models_config_.find(id);
			return iter != models_config_.end();
		}

		void discard_model(int id) {
			auto iter = models_config_.find(id);
			if (iter != models_config_.end()) {
				models_config_.erase(iter);
			}
		}
	};

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
		Merger merger_;					 // creates a background thread that keep merging processor output
		Processor processor_;		 // creates a background thread for processing (for now only one thread)
		
  };

} // namespace topicmd
#endif // TOPICMD_INSTANCE_
