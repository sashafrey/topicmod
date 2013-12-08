#ifndef TOPICMD_PROCESSOR_
#define TOPICMD_PROCESSOR_

#include <memory>
#include <queue>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>

#include "topicmd/partition.h"
#include "topicmd/merger.h"

namespace topicmd {
  class Processor : boost::noncopyable {
  public:
    Processor(boost::mutex& processor_queue_lock,
	      std::queue<std::shared_ptr<const Partition> >&  processor_queue,
	      boost::mutex& merger_queue_lock,
	      std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
	      const Merger& merger) :
      processor_queue_lock_(processor_queue_lock),
      processor_queue_(processor_queue),
      merger_queue_lock_(merger_queue_lock),
      merger_queue_(merger_queue),
      merger_(merger),
      thread_(boost::bind(&Processor::ThreadFunction, this))
    {
    }

    ~Processor() {
      if (thread_.joinable()) {
	thread_.interrupt();
	thread_.join();
      }
    }

    void Join() {
      thread_.join();
    }
  private:
    boost::mutex& processor_queue_lock_;
    std::queue<std::shared_ptr<const Partition> >& processor_queue_;
    boost::mutex& merger_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue_;
    const Merger& merger_;

    boost::thread thread_;
    void ThreadFunction();
  };
} // topicmd

#endif // TOPICMD_PROCESSOR_
