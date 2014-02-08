#include "instance_manager.h"

namespace topicmd {

int InstanceManager::CreateInstance(int id, const InstanceConfig& config) {
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
    
bool InstanceManager::has_instance(int id) const {
  boost::lock_guard<boost::mutex> guard(lock_);      
  return instance_map_.find(id) != instance_map_.end();
}

const std::shared_ptr<Instance> InstanceManager::instance(int id) const {
  boost::lock_guard<boost::mutex> guard(lock_);
  auto iter = instance_map_.find(id);
  return iter->second;
}

std::shared_ptr<Instance> InstanceManager::instance(int id) {
  boost::lock_guard<boost::mutex> guard(lock_);
  auto iter = instance_map_.find(id);
  return iter->second;
}

void InstanceManager::erase_instance(int id) {
  boost::lock_guard<boost::mutex> guard(lock_);
  instance_map_.erase(id);
}

void InstanceManager::clear() {
  boost::lock_guard<boost::mutex> guard(lock_);
  instance_map_.clear();
}

}
