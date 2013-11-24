#ifndef TOPICMD_INSTANCE_MANAGER_
#define TOPICMD_INSTANCE_MANAGER_

#include <memory>

#include "topicmd/common.h"
#include "topicmd/instance.h"
#include "topicmd/messages.pb.h"

namespace topicmd {

  class InstanceManager {
  public:
    static InstanceManager& get() {
      static InstanceManager instance_manager;
      return instance_manager;
    }

    int CreateInstance(int id, const InstanceConfig& config) {
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

      std::pair<int, Instance::Ptr>
	value(id, Instance::Ptr(new Instance(id, config)));

      instance_map_.insert(value);
      return id;
    }
    
    bool has_instance(int id) const {
      return instance_map_.find(id) != instance_map_.end();
    }

    const Instance& instance(int id) const {
      auto iter = instance_map_.find(id);
      return *(iter->second);
    }

    Instance& mutable_instance(int id) {
      auto iter = instance_map_.find(id);
      return *(iter->second);
    }

    void erase_instance(int id) {
      instance_map_.erase(id);
    }

    void clear() {
      instance_map_.clear();
    }
  private:
    // Singleton (make constructor private)
    InstanceManager() : next_id_(1) { };      // Don't Implement
    InstanceManager(const InstanceManager& );  // Don't Implement
    void operator=(const InstanceManager& );   // Don't implement

    int next_id_;
    std::map<int, Instance::Ptr> instance_map_;
  };

}

#endif // TOPICMD_INSTANCE_MANAGER_
