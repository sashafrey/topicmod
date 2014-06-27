// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_MASTER_COMPONENT_SERVICE_IMPL_H_
#define SRC_ARTM_CORE_MASTER_COMPONENT_SERVICE_IMPL_H_

#include <map>
#include <memory>
#include <string>

#include "boost/thread/mutex.hpp"

#include "rpcz/service.hpp"

#include "artm/messages.pb.h"
#include "artm/core/batch_manager.h"
#include "artm/core/common.h"
#include "artm/core/internals.pb.h"
#include "artm/core/internals.rpcz.h"
#include "artm/core/topic_model.h"
#include "artm/core/thread_safe_holder.h"

namespace zmq {
class context_t;
}  // namespace zmq

namespace rpcz {
class application;
}  // namespace rpcz

namespace artm {
namespace core {

class NetworkClientCollection;

class MasterComponentServiceImpl : public MasterComponentService {
 public:
  explicit MasterComponentServiceImpl(NetworkClientCollection* clients);

  ~MasterComponentServiceImpl() { ; }

  virtual void UpdateModel(const ::artm::core::ModelIncrement& request,
                       ::rpcz::reply< ::artm::core::Void> response);
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


  void InvokeIteration(int iterations_count, std::string disk_path);
  void WaitIdle();
  bool RequestTopicModel(ModelName model_name, ::artm::TopicModel* topic_model);

 private:
  mutable boost::mutex lock_;
  BatchManager batch_manager_;
  ThreadSafeCollectionHolder<::artm::core::ModelName, ::artm::core::TopicModel> topic_model_;
  std::unique_ptr<rpcz::application> application_;
  NetworkClientCollection* clients_;
};

}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_CORE_MASTER_COMPONENT_SERVICE_IMPL_H_
