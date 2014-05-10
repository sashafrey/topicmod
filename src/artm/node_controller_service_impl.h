// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_NODE_CONTROLLER_SERVICE_IMPL_H_
#define SRC_ARTM_NODE_CONTROLLER_SERVICE_IMPL_H_

#include <map>
#include <memory>

#include "boost/thread/mutex.hpp"

#include "artm/common.h"
#include "artm/internals.pb.h"
#include "artm/internals.rpcz.h"
#include "artm/topic_model.h"

#include "rpcz/service.hpp"

namespace artm {
namespace core {

class NodeControllerServiceImpl : public NodeControllerService {
 public:
  NodeControllerServiceImpl() : lock_() { ; }
  ~NodeControllerServiceImpl() { ; }

  virtual void CreateOrReconfigureInstance(const ::artm::InstanceConfig& request,
                       ::rpcz::reply< ::artm::core::Int> response);
  virtual void DisposeInstance(const ::artm::core::Int& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureDataLoader(const ::artm::DataLoaderConfig& request,
                       ::rpcz::reply< ::artm::core::Int> response);
  virtual void DisposeDataLoader(const ::artm::core::Int& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureModel(const ::artm::core::CreateOrReconfigureModelArgs& request,
                       ::rpcz::reply< ::artm::core::Int> response);
  virtual void DisposeModel(const ::artm::core::DisposeModelArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureRegularizer(const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
                       ::rpcz::reply< ::artm::core::Int> response);
  virtual void DisposeRegularizer(const ::artm::core::DisposeRegularizerArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);

 private:
  mutable boost::mutex lock_;
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_NODE_CONTROLLER_SERVICE_IMPL_H_
