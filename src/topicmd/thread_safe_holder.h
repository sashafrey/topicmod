#ifndef THREAD_SAFE_HOLDER_
#define THREAD_SAFE_HOLDER_

#include <memory>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

namespace topicmd {

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

    std::shared_ptr<T> get() {
      boost::lock_guard<boost::mutex> guard(lock_);
      return get_locked();
    }

    // Use this instead of get() when the lock is already acquired.
    std::shared_ptr<T> get_locked() const {
      return object_;
    }

    const std::shared_ptr<T>& get() const {
      boost::lock_guard<boost::mutex> guard(lock_);
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

} // namespace topicmd

#endif // THREAD_SAFE_HOLDER_