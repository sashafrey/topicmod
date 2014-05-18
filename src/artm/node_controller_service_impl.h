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
  NodeControllerServiceImpl() : lock_(), instance_id_(kUndefinedId), data_loader_id_(kUndefinedId) { ; }
  ~NodeControllerServiceImpl();

  virtual void CreateOrReconfigureInstance(const ::artm::InstanceConfig& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeInstance(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureDataLoader(const ::artm::DataLoaderConfig& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeDataLoader(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureModel(const ::artm::core::CreateOrReconfigureModelArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeModel(const ::artm::core::DisposeModelArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureRegularizer(const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeRegularizer(const ::artm::core::DisposeRegularizerArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);

 private:
  static const int kUndefinedId = -1;
  mutable boost::mutex lock_;

  // Currently node controller supports only one Instance and one DataLoader per node.
  // This makes it simpler to implement MasterComponent --- it doesn't have to keep track
  // of instance_id and data_loader_id for each of its clients.
  int instance_id_;
  int data_loader_id_;
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_NODE_CONTROLLER_SERVICE_IMPL_H_
