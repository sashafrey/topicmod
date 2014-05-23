// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_MASTER_COMPONENT_SERVICE_IMPL_H_
#define SRC_ARTM_MASTER_COMPONENT_SERVICE_IMPL_H_

#include <map>
#include <memory>

#include "boost/thread/mutex.hpp"

#include "rpcz/application.hpp"
#include "rpcz/service.hpp"

#include "artm/common.h"
#include "artm/internals.pb.h"
#include "artm/internals.rpcz.h"
#include "artm/topic_model.h"
#include "artm/thread_safe_holder.h"

namespace zmq {
class context_t;
}  // namespace zmq

namespace artm {
namespace core {

class NetworkClientCollection;

class MasterComponentServiceImpl : public MasterComponentService {
 public:
  explicit MasterComponentServiceImpl(NetworkClientCollection* clients);

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
  mutable boost::mutex lock_;
  std::map<::artm::core::ModelName, std::shared_ptr<::artm::core::TopicModel>> topic_model_;
  std::unique_ptr<rpcz::application> application_;
  NetworkClientCollection* clients_;
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_MASTER_COMPONENT_SERVICE_IMPL_H_
