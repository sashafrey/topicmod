#ifndef DATA_LOADER_
#define DATA_LOADER_

#include <boost/thread.hpp>   
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/generation.h"
#include "topicmd/messages.pb.h"
#include "topicmd/partition.h"
#include "topicmd/thread_safe_holder.h"

namespace topicmd {
  class DataLoader : boost::noncopyable {
  public:
    DataLoader(boost::mutex& lock, 
	       std::queue<std::shared_ptr<const Partition> >& queue,
	       ThreadSafeHolder<Generation>& generation) :
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

		void Interrupt() {
			thread_.interrupt();
		}

    void Join() {
			thread_.join();
    }
  private:
    boost::mutex& lock_;
    std::queue<std::shared_ptr<const Partition> >& queue_;
    ThreadSafeHolder<Generation>& generation_;

    // Keep all threads at the end of class members
    // (because the order of class members defines initialization order;
    // everything else should be initialized before creating threads).
    boost::thread thread_;

    void ThreadFunction() 
    {
      try {
				for (;;)
				{
					// Sleep and check for interrupt.
					// To check for interrupt without sleep,
					// use boost::this_thread::interruption_point()
					// which also throws boost::thread_interrupted
					boost::this_thread::sleep(boost::posix_time::milliseconds(1));

					{
						boost::lock_guard<boost::mutex> guard(lock_);
						if (queue_.size() > 10) {
							continue; 
						}
					}

					{
						auto latest_generation = generation_.get();

						boost::lock_guard<boost::mutex> guard(lock_);
						latest_generation->InvokeOnEachPartition([&](std::shared_ptr<const Partition> part) { 
							queue_.push(part);
						});
					}
				}
			}
			catch(boost::thread_interrupted&) {
				return;
			}
		}
  };
}

#endif // DATA_LOADER_