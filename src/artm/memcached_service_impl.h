// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MEMCACHED_SERVICE_IMPL_H_
#define SRC_ARTM_MEMCACHED_SERVICE_IMPL_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "boost/thread/mutex.hpp"

#include "artm/common.h"
#include "artm/memcached_service.pb.h"
#include "artm/memcached_service.rpcz.h"
#include "artm/thread_safe_holder.h"
#include "artm/topic_model.h"

#include "rpcz/service.hpp"

namespace artm {
namespace memcached {

// A draft implementation of memcached service.
class MemcachedServiceImpl : public MemcachedService {
 public:
  MemcachedServiceImpl() : topic_model_(), lock_() { ; }
  ~MemcachedServiceImpl() { ; }

  virtual void UpdateModel(const ::artm::core::ModelIncrement& request,
                       ::rpcz::reply< ::artm::TopicModel> response);
  virtual void RetrieveModel(const ::artm::memcached::ModelId& request,
                       ::rpcz::reply< ::artm::TopicModel> response);

 private:
  std::map<::artm::core::ModelId, std::shared_ptr<::artm::core::TopicModel>> topic_model_;
  mutable boost::mutex lock_;
};

}  // namespace memcached
}  // namespace artm


#endif  // SRC_ARTM_MEMCACHED_SERVICE_IMPL_H_
