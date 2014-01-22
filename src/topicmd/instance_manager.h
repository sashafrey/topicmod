#ifndef TOPICMD_INSTANCE_MANAGER_
#define TOPICMD_INSTANCE_MANAGER_

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

    int CreateInstance(int id, const InstanceConfig& config) {
      boost::lock_guard<boost::mutex> guard(lock_);
      if (id <= 0) {
				// iterate through instance_map_ until find some slot
				while(instance_map_.find(next_id_) != instance_map_.end()) {
					next_id_++;
				}

				id = next_id_++;
      }

      if (instance_map_.find(id) != instance_map_.end()) {
				return TOPICMD_ERROR;
      }

      instance_map_.insert(std::make_pair(
        id, std::make_shared<Instance>(id, config)));

      return id;
    }
    
    bool has_instance(int id) const {
      boost::lock_guard<boost::mutex> guard(lock_);      
      return instance_map_.find(id) != instance_map_.end();
    }

    const std::shared_ptr<Instance> instance(int id) const {
      boost::lock_guard<boost::mutex> guard(lock_);
      auto iter = instance_map_.find(id);
      return iter->second;
    }

    std::shared_ptr<Instance> instance(int id) {
      boost::lock_guard<boost::mutex> guard(lock_);
      auto iter = instance_map_.find(id);
      return iter->second;
    }

    void erase_instance(int id) {
      boost::lock_guard<boost::mutex> guard(lock_);
      instance_map_.erase(id);
    }

    void clear() {
      boost::lock_guard<boost::mutex> guard(lock_);
      instance_map_.clear();
    }
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
