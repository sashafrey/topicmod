// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MASTER_COMPONENT_SERVICE_IMPL_H_
#define SRC_ARTM_MASTER_COMPONENT_SERVICE_IMPL_H_

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

class MasterComponentServiceImpl : public MasterComponentService {
 public:
  MasterComponentServiceImpl() : topic_model_(), lock_() { ; }
  ~MasterComponentServiceImpl() { ; }

  virtual void UpdateModel(const ::artm::core::ModelIncrement& request,
                       ::rpcz::reply< ::artm::TopicModel> response);
  virtual void RetrieveModel(const ::artm::core::String& request,
                       ::rpcz::reply< ::artm::TopicModel> response);

  virtual void RequestBatches(const ::artm::core::Int& request,
                       ::rpcz::reply< ::artm::core::BatchIds> response);
  virtual void ReportBatches(const ::artm::core::BatchIds& request,
                       ::rpcz::reply< ::artm::core::Void> response);

  virtual void ConnectClient(const ::artm::core::String& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisconnectClient(const ::artm::core::String& request,
                       ::rpcz::reply< ::artm::core::Void> response);


 private:
  std::map<::artm::core::ModelId, std::shared_ptr<::artm::core::TopicModel>> topic_model_;
  mutable boost::mutex lock_;
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_MASTER_COMPONENT_SERVICE_IMPL_H_
