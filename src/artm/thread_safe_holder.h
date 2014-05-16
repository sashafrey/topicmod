// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_THREAD_SAFE_HOLDER_H_
#define SRC_ARTM_THREAD_SAFE_HOLDER_H_

#include <queue>
#include <map>
#include <memory>
#include <utility>

#include "boost/thread/locks.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"

#include "artm/common.h"

namespace artm {
namespace core {

// A helper-class, which magically turns any class into thread-safe thing.
// The only requirement: the class must have deep copy constructor.
// The key idea is in const get() method, which returns const shared_ptr<T>.
// This object can be further used without any locks, assuming that all
// access is read-only. In the meantime the object in ThreadSafeHolder
// might be replaced with a new instance (via set() method).
template<typename T>
class ThreadSafeHolder : boost::noncopyable {
 public:
  explicit ThreadSafeHolder(boost::mutex& lock)  // NOLINT
      : lock_(lock), object_(std::make_shared<T>()) {}

  ThreadSafeHolder(boost::mutex& lock, const std::shared_ptr<T>& object)  // NOLINT
      : lock_(lock), object_(object) {}

  ~ThreadSafeHolder() {}

  std::shared_ptr<T> get() const {
    boost::lock_guard<boost::mutex> guard(lock_);
    return get_locked();
  }

  // Use this instead of get() when the lock is already acquired.
  std::shared_ptr<T> get_locked() const {
    return object_;
  }

  std::shared_ptr<T> get_copy() const {
    boost::lock_guard<boost::mutex> guard(lock_);
    return std::make_shared<T>(*object_);
  }

  void set(const std::shared_ptr<T>& object) {
    boost::lock_guard<boost::mutex> guard(lock_);
    object_ = object;
  }

 private:
  boost::mutex& lock_;
  std::shared_ptr<T> object_;
};

template<typename K, typename T>
class ThreadSafeCollectionHolder : boost::noncopyable {
 public:
  explicit ThreadSafeCollectionHolder(boost::mutex& lock) :  // NOLINT
    lock_(lock), object_(std::map<K, std::shared_ptr<T>>()) {}

  ~ThreadSafeCollectionHolder() {}

  std::shared_ptr<T> get(const K& key) const {
    boost::lock_guard<boost::mutex> guard(lock_);
    return get_locked(key);
  }

  bool has_key(const K& key) {
    boost::lock_guard<boost::mutex> guard(lock_);
    return object_.find(key) != object_.end();
  }

  void erase(const K& key) {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto iter = object_.find(key);
    if (iter != object_.end()) {
      object_.erase(iter);
    }
  }

  // Use this instead of get() when the lock is already acquired.
  std::shared_ptr<T> get_locked(const K& key) const {
    auto iter = object_.find(key);
    return (iter != object_.end()) ? iter->second : std::shared_ptr<T>();
  }

  std::shared_ptr<T> get_copy(const K& key) const {
    auto value = get(key);
    return value != nullptr ? std::make_shared<T>(*value) : std::shared_ptr<T>();
  }

  void set(const K& key, const std::shared_ptr<T>& object) {
    boost::lock_guard<boost::mutex> guard(lock_);
    auto iter = object_.find(key);
    if (iter != object_.end()) {
      iter->second = object;
    } else {
      object_.insert(std::pair<K, std::shared_ptr<T> >(key, object));
    }
  }

 private:
  boost::mutex& lock_;
  std::map<K, std::shared_ptr<T> > object_;
};

template<typename T>
class ThreadSafeQueue : boost::noncopyable {
 public:
  bool try_pop(T* elem) {
    boost::lock_guard<boost::mutex> guard(lock_);
    if (queue_.empty())
      return false;

    T tmp_elem = queue_.front();
    queue_.pop();
    *elem = tmp_elem;
    return true;
  }

  void push(const T& elem) {
    boost::lock_guard<boost::mutex> guard(lock_);
    queue_.push(elem);
  }
 private:
  boost::mutex lock_;
  std::queue<T> queue_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_THREAD_SAFE_HOLDER_H_
