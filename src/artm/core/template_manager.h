// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_TEMPLATE_MANAGER_H_
#define SRC_ARTM_CORE_TEMPLATE_MANAGER_H_

#include <map>
#include <memory>

#include "boost/thread/locks.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "artm/messages.pb.h"

#include "artm/core/common.h"

namespace artm {
namespace core {

// Singleton class to manage a collection of objects, identifiable with some integer ID.
template<class Type, class Config>
class TemplateManager : boost::noncopyable {
 public:
  static TemplateManager<Type, Config>& singleton() {
    // Mayers singleton is thread safe in C++11
    // http://stackoverflow.com/questions/1661529/is-meyers-implementation-of-singleton-pattern-thread-safe
    static TemplateManager<Type, Config> manager;
    return manager;
  }

  // Tries to create an object with given id.
  // Return: true if id() was available and object was created successfully, and false otherwise.
  template<typename Derrived>
  bool TryCreate(int id, const Config& config) {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (map_.find(id) != map_.end()) {
      return false;
    }

    std::shared_ptr<Type> ptr(new Derrived(id, config));
    map_.insert(std::make_pair(id, ptr));
    return true;
  }

  bool TryCreate(int id, const Config& config) {
    return TryCreate<Type>(id, config);
  }

  // Create an object and returns its ID.
  template<typename Derrived>
  int Create(const Config& config) {
    boost::lock_guard<boost::mutex> guard(lock_);

    // iterate through instance_map_ until find an available slot
    while (map_.find(next_id_) != map_.end()) {
      next_id_++;
    }

    int id = next_id_++;
    std::shared_ptr<Type> ptr(new Derrived(id, config));
    map_.insert(std::make_pair(id, ptr));
    return id;
  }

  int Create(const Config& config) {
    return Create<Type>(config);
  }

  template<typename Derrived>
  const std::shared_ptr<Derrived> Get(int id) const {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto iter = map_.find(id);
    return (iter == map_.end()) ? nullptr : std::dynamic_pointer_cast<Derrived>(iter->second);
  }

  const std::shared_ptr<Type> Get(int id) const {
    return Get<Type>(id);
  }

  template<typename Derrived>
  std::shared_ptr<Derrived> Get(int id) {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto iter = map_.find(id);
    return (iter == map_.end()) ? nullptr : std::dynamic_pointer_cast<Derrived>(iter->second);
  }

  const std::shared_ptr<Type> Get(int id) {
    return Get<Type>(id);
  }

  template<typename Derrived>
  std::shared_ptr<Derrived> First() {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto iter = map_.begin();
    if (iter != map_.end()) {
      return std::dynamic_pointer_cast<Derrived>(iter->second);
    } else {
      return std::shared_ptr<Derrived>();
    }
  }

  std::shared_ptr<Type> First() {
    return First<Type>();
  }

  void Erase(int id) {
    boost::lock_guard<boost::mutex> guard(lock_);
    map_.erase(id);
  }

  void Clear() {
    boost::lock_guard<boost::mutex> guard(lock_);
    map_.clear();
  }

 private:
  // Singleton (make constructor private)
  TemplateManager() : lock_(), next_id_(1) {}

  mutable boost::mutex lock_;

  int next_id_;
  std::map<int, std::shared_ptr<Type> > map_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_TEMPLATE_MANAGER_H_
