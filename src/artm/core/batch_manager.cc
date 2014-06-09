// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/batch_manager.h"

namespace artm {
namespace core {

BatchManager::BatchManager(boost::mutex* lock)
    : lock_(lock), tasks_(), in_progress_() {}

void BatchManager::Add(const boost::uuids::uuid& id) {
  boost::lock_guard<boost::mutex> guard(*lock_);
  tasks_.push_back(id);
}

boost::uuids::uuid BatchManager::Next() {
  boost::lock_guard<boost::mutex> guard(*lock_);
  for (auto iter = tasks_.begin(); iter != tasks_.end(); ++iter) {
    if (in_progress_.find(*iter) == in_progress_.end()) {
      boost::uuids::uuid retval = *iter;
      tasks_.erase(iter);
      in_progress_.insert(retval);
      return retval;
    }
  }

  return boost::uuids::uuid();
}

void BatchManager::Done(const boost::uuids::uuid& id) {
  boost::lock_guard<boost::mutex> guard(*lock_);
  in_progress_.erase(id);
}

bool BatchManager::IsEverythingProcessed() const {
  boost::lock_guard<boost::mutex> guard(*lock_);
  return (tasks_.empty() && in_progress_.empty());
}

}  // namespace core
}  // namespace artm
