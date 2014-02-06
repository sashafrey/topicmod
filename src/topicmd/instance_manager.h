#ifndef TOPICMD_INSTANCE_MANAGER_
#define TOPICMD_INSTANCE_MANAGER_

#include <map>
#include <memory>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/common.h"
#include "topicmd/instance.h"
#include "topicmd/messages.pb.h"

namespace topicmd {

  class InstanceManager : boost::noncopyable {
  public:
    static InstanceManager& singleton() {
      // Mayers singleton is thread safe in C++11
      // http://stackoverflow.com/questions/1661529/is-meyers-implementation-of-singleton-pattern-thread-safe
      static InstanceManager instance_manager;
      return instance_manager;
    }

    int CreateInstance(int id, const InstanceConfig& config);

    bool has_instance(int id) const;
    const std::shared_ptr<Instance> instance(int id) const;
    std::shared_ptr<Instance> instance(int id);
    void erase_instance(int id);
    void clear();

  private:
    // Singleton (make constructor private)
    InstanceManager() :
      lock_(),
      next_id_(1) { };

    mutable boost::mutex lock_;

    int next_id_;
    std::map<int, std::shared_ptr<Instance> > instance_map_;
  };

}

#endif // TOPICMD_INSTANCE_MANAGER_
