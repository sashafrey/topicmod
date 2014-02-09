#ifndef THREAD_SAFE_HOLDER_
#define THREAD_SAFE_HOLDER_

#include <map>
#include <memory>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

namespace artm { namespace core {

  // A helper-class, which magically turns any class into thread-safe thing.
  // The only requirement: the class must have deep copy constructor.
  // The key idea is in const get() method, which returns const shared_ptr<T>.
  // This object can be further used without any locks, assuming that all 
  // access is read-only. In the meantime the object in ThreadSafeHolder
  // might be replaced with a new instance (via set() method).
  template<typename T>
  class ThreadSafeHolder : boost::noncopyable {
  public:
    explicit ThreadSafeHolder(boost::mutex& lock) :
      lock_(lock), object_(std::make_shared<T>()) 
    {
    }

    ThreadSafeHolder(boost::mutex& lock, const std::shared_ptr<T>& object)
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
    
    void set(const std::shared_ptr<T>& object)
    {
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
    explicit ThreadSafeCollectionHolder(boost::mutex& lock) :
      lock_(lock), object_(std::map<K, std::shared_ptr<T>>()) 
    {
    }

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
      auto iter = object_.find(key);
      return (iter != object_.end()) ? std::make_shared<T>(*(iter->second)) : std::shared_ptr<T>();
    }
    
    void set(const K& key, const std::shared_ptr<T>& object)
    {
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

}} // namespace artm/core

#endif // THREAD_SAFE_HOLDER_
