#ifndef TOPICMD_INSTANCE_
#define TOPICMD_INSTANCE_

#include <map>
#include <memory>
#include <queue>
#include <vector>
    
#include <boost/thread.hpp>   
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/merger.h"
#include "topicmd/messages.pb.h"
#include "topicmd/partition.h"
#include "topicmd/thread_safe_holder.h"

namespace topicmd {

  class DataLoader : boost::noncopyable {
  public:
    DataLoader(boost::mutex& lock, 
	       std::queue<std::shared_ptr<const Partition> >& queue,
	       std::shared_ptr<const Generation> generation) :
      lock_(lock), 
      queue_(queue),
      generation_(generation),
      thread_(boost::bind(&DataLoader::ThreadFunction, this))
    {
    }

    ~DataLoader() {
      if (thread_.joinable()) {
	thread_.interrupt();
	thread_.join();
      }
    }

    void Join() {
      thread_.join();
    }
  private:
    boost::mutex& lock_;
    std::queue<std::shared_ptr<const Partition> >& queue_;
    std::shared_ptr<const Generation> generation_;

    // Keep all threads at the end of class members
    // (because the order of class members defines initialization order;
    // everything else should be initialized before creating threads).
    boost::thread thread_;

    void ThreadFunction() 
    {
      try {
	{
	  boost::lock_guard<boost::mutex> guard(lock_);
	  generation_->InvokeOnEachPartition([&](std::shared_ptr<const Partition> part) { 
	      queue_.push(part);
	    });
	}


	// Sleep and check for interrupt.
	// To check for interrupt without sleep,
	// use boost::this_thread::interruption_point()
	// which also throws boost::thread_interrupted
	// boost::this_thread::sleep(boost::posix_time::milliseconds(50));
      }
      catch(boost::thread_interrupted&) {
	return;
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

    const std::shared_ptr<InstanceConfig> config() const {
      return instance_config_.get();
    }

    int DiscardPartition();
    int FinishPartition();
    int GetTotalItemsCount() const;
    int InsertBatch(const Batch& batch);
    int PublishGeneration(int generation_id);
    int Reconfigure(const InstanceConfig& config);
    int RunTuningIteration();

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
    ThreadSafeHolder<InstanceConfig> instance_config_;

    // ToDo: processor queue must have not parts, 
    // but special processing batches, self-containing data structures
    // to process them in a processor.
    mutable boost::mutex processor_queue_lock_;
    std::queue<std::shared_ptr<const Partition> > processor_queue_;

    mutable boost::mutex merger_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorOutput> > merger_queue_;
  };

} // namespace topicmd
#endif // TOPICMD_INSTANCE_
