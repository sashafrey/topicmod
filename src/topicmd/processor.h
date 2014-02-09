#ifndef TOPICMD_PROCESSOR_
#define TOPICMD_PROCESSOR_

#include <memory>
#include <queue>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>

#include "topicmd/instance_schema.h"
#include "topicmd/internals.pb.h"
#include "topicmd/merger.h"
#include "topicmd/thread_safe_holder.h"

namespace artm { namespace core {

  class Processor : boost::noncopyable {
  public:
    Processor(boost::mutex& processor_queue_lock,
        std::queue<std::shared_ptr<const Batch> >&  processor_queue,
        boost::mutex& merger_queue_lock,
        std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
        const Merger& merger,
        ThreadSafeHolder<InstanceSchema>& schema) :
      processor_queue_lock_(processor_queue_lock),
      processor_queue_(processor_queue),
      merger_queue_lock_(merger_queue_lock),
      merger_queue_(merger_queue),
      merger_(merger),
      schema_(schema),
      thread_(boost::bind(&Processor::ThreadFunction, this))
    {
    }

    ~Processor();
    void Interrupt();
    void Join();
  private:
    boost::mutex& processor_queue_lock_;
    std::queue<std::shared_ptr<const Batch> >& processor_queue_;
    boost::mutex& merger_queue_lock_;
    std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue_;
    const Merger& merger_;
    ThreadSafeHolder<InstanceSchema>& schema_;

    boost::thread thread_;
    void ThreadFunction();
  };
}} // artm/core

#endif // TOPICMD_PROCESSOR_
