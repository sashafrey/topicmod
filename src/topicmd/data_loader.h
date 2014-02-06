#ifndef DATA_LOADER_
#define DATA_LOADER_

#include <queue>

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

    ~DataLoader();
    void Interrupt();
    void Join();

  private:
    boost::mutex& lock_;
    std::queue<std::shared_ptr<const Partition> >& queue_;
    ThreadSafeHolder<Generation>& generation_;

    // Keep all threads at the end of class members
    // (because the order of class members defines initialization order;
    // everything else should be initialized before creating threads).
    boost::thread thread_;

    void ThreadFunction();
  };
}

#endif // DATA_LOADER_