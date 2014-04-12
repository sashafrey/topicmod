// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MEMCACHED_SERVICE_IMPL_H_
#define SRC_ARTM_MEMCACHED_SERVICE_IMPL_H_

#include <map>
#include <string>
#include <vector>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"

#include "artm/memcached_service.pb.h"
#include "artm/memcached_service.rpcz.h"

#include "rpcz/service.hpp"

namespace artm {
namespace memcached {

// A draft implementatino of memcached service.
// ToDo(alfrey): replace this with real memcached (http://memcached.org/)
class MemcachedServiceImpl : public MemcachedService {
 public:
  MemcachedServiceImpl() : lock_() { ; }
  ~MemcachedServiceImpl() { ; }

  virtual void UpdateKey(const UpdateKeyArgs& request,
                         ::rpcz::reply<UpdateKeyResult> response);

  virtual void RetrieveKey(const RetrieveKeyArgs& request,
                           ::rpcz::reply<RetrieveKeyResult> response);

  virtual void RetrieveGroup(const RetrieveGroupArgs& request,
                             ::rpcz::reply<RetrieveGroupResult> response);

 private:
  typedef float ValueType;
  typedef std::string KeyType;
  typedef std::string KeyGroupType;

  typedef std::vector<ValueType> ValueStore;
  typedef std::map<KeyType, ValueStore> KeyStore;
  typedef std::map<KeyGroupType, KeyStore> GroupStore;

  GroupStore data_;
  mutable boost::mutex lock_;
};

}  // namespace memcached
}  // namespace artm


#endif  // SRC_ARTM_MEMCACHED_SERVICE_IMPL_H_
