// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: artm/core/internals.proto

#ifndef RPCZ_artm_2fcore_2finternals_2eproto__INCLUDED
#define RPCZ_artm_2fcore_2finternals_2eproto__INCLUDED

#include <string>
#include <rpcz/service.hpp>

namespace google {
namespace protobuf {
class ServiceDescriptor;
class MethodDescriptor;
}  // namespace protobuf
}  // namespace google
namespace rpcz {
class rpc;
class closure;
class rpc_channel;
}  //namesacpe rpcz
#include "artm/messages.pb.h"
#include "artm/internals.pb.h"

namespace artm {
namespace core {
void rpcz_protobuf_AssignDesc_artm_2fcore_2finternals_2eproto();
void rpcz_protobuf_ShutdownFile_artm_2fcore_2finternals_2eproto();

class MasterComponentService_Stub;

class MasterComponentService : public rpcz::service {
 protected:
  // This class should be treated as an abstract interface.
  inline MasterComponentService() {};
 public:
  virtual ~MasterComponentService();

  typedef MasterComponentService_Stub Stub;

  static const ::google::protobuf::ServiceDescriptor* descriptor();

  virtual void UpdateModel(const ::artm::core::ModelIncrement& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void RetrieveModel(const ::artm::core::String& request,
                       ::rpcz::reply< ::artm::TopicModel> response);
  virtual void RequestBatches(const ::artm::core::Int& request,
                       ::rpcz::reply< ::artm::core::BatchIds> response);
  virtual void ReportBatches(const ::artm::core::BatchIds& request,
                       ::rpcz::reply< ::artm::core::Void> response);

  // implements Service ----------------------------------------------

  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void call_method(const ::google::protobuf::MethodDescriptor* method,
                  const ::google::protobuf::Message& request,
                  ::rpcz::server_channel* channel);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MasterComponentService);
};

class MasterComponentService_Stub {
 public:
  MasterComponentService_Stub(::rpcz::rpc_channel* channel, 
                   bool owns_channel=false);
  MasterComponentService_Stub(::rpcz::rpc_channel* channel, 
                   const ::std::string& service_name,
                   bool owns_channel=false);
  ~MasterComponentService_Stub();

  inline ::rpcz::rpc_channel* channel() { return channel_; }


  void UpdateModel(const ::artm::core::ModelIncrement& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void UpdateModel(const ::artm::core::ModelIncrement& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void RetrieveModel(const ::artm::core::String& request,
                       ::artm::TopicModel* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void RetrieveModel(const ::artm::core::String& request,
                       ::artm::TopicModel* response,
                       long deadline_ms = -1);
  void RequestBatches(const ::artm::core::Int& request,
                       ::artm::core::BatchIds* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void RequestBatches(const ::artm::core::Int& request,
                       ::artm::core::BatchIds* response,
                       long deadline_ms = -1);
  void ReportBatches(const ::artm::core::BatchIds& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void ReportBatches(const ::artm::core::BatchIds& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
 private:
  ::rpcz::rpc_channel* channel_;
  ::std::string service_name_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MasterComponentService_Stub);
};

class NodeControllerService_Stub;

class NodeControllerService : public rpcz::service {
 protected:
  // This class should be treated as an abstract interface.
  inline NodeControllerService() {};
 public:
  virtual ~NodeControllerService();

  typedef NodeControllerService_Stub Stub;

  static const ::google::protobuf::ServiceDescriptor* descriptor();

  virtual void CreateOrReconfigureInstance(const ::artm::MasterComponentConfig& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeInstance(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureMasterComponent(const ::artm::MasterComponentConfig& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeMasterComponent(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureModel(const ::artm::core::CreateOrReconfigureModelArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeModel(const ::artm::core::DisposeModelArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureRegularizer(const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeRegularizer(const ::artm::core::DisposeRegularizerArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void CreateOrReconfigureDictionary(const ::artm::core::CreateOrReconfigureDictionaryArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void DisposeDictionary(const ::artm::core::DisposeDictionaryArgs& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void ForcePullTopicModel(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void ForcePushTopicModelIncrement(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void OverwriteTopicModel(const ::artm::TopicModel& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void RequestTopicModel(const ::artm::core::String& request,
                       ::rpcz::reply< ::artm::TopicModel> response);
  virtual void RequestThetaMatrix(const ::artm::core::String& request,
                       ::rpcz::reply< ::artm::ThetaMatrix> response);
  virtual void RequestScore(const ::artm::core::RequestScoreArgs& request,
                       ::rpcz::reply< ::artm::ScoreData> response);
  virtual void AddBatch(const ::artm::Batch& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void InvokeIteration(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void WaitIdle(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);
  virtual void InvokePhiRegularizers(const ::artm::core::Void& request,
                       ::rpcz::reply< ::artm::core::Void> response);

  // implements Service ----------------------------------------------

  const ::google::protobuf::ServiceDescriptor* GetDescriptor();
  void call_method(const ::google::protobuf::MethodDescriptor* method,
                  const ::google::protobuf::Message& request,
                  ::rpcz::server_channel* channel);
  const ::google::protobuf::Message& GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const;
  const ::google::protobuf::Message& GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const;

 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(NodeControllerService);
};

class NodeControllerService_Stub {
 public:
  NodeControllerService_Stub(::rpcz::rpc_channel* channel, 
                   bool owns_channel=false);
  NodeControllerService_Stub(::rpcz::rpc_channel* channel, 
                   const ::std::string& service_name,
                   bool owns_channel=false);
  ~NodeControllerService_Stub();

  inline ::rpcz::rpc_channel* channel() { return channel_; }


  void CreateOrReconfigureInstance(const ::artm::MasterComponentConfig& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void CreateOrReconfigureInstance(const ::artm::MasterComponentConfig& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void DisposeInstance(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void DisposeInstance(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void CreateOrReconfigureMasterComponent(const ::artm::MasterComponentConfig& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void CreateOrReconfigureMasterComponent(const ::artm::MasterComponentConfig& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void DisposeMasterComponent(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void DisposeMasterComponent(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void CreateOrReconfigureModel(const ::artm::core::CreateOrReconfigureModelArgs& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void CreateOrReconfigureModel(const ::artm::core::CreateOrReconfigureModelArgs& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void DisposeModel(const ::artm::core::DisposeModelArgs& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void DisposeModel(const ::artm::core::DisposeModelArgs& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void CreateOrReconfigureRegularizer(const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void CreateOrReconfigureRegularizer(const ::artm::core::CreateOrReconfigureRegularizerArgs& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void DisposeRegularizer(const ::artm::core::DisposeRegularizerArgs& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void DisposeRegularizer(const ::artm::core::DisposeRegularizerArgs& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void CreateOrReconfigureDictionary(const ::artm::core::CreateOrReconfigureDictionaryArgs& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void CreateOrReconfigureDictionary(const ::artm::core::CreateOrReconfigureDictionaryArgs& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void DisposeDictionary(const ::artm::core::DisposeDictionaryArgs& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void DisposeDictionary(const ::artm::core::DisposeDictionaryArgs& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void ForcePullTopicModel(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void ForcePullTopicModel(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void ForcePushTopicModelIncrement(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void ForcePushTopicModelIncrement(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void OverwriteTopicModel(const ::artm::TopicModel& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void OverwriteTopicModel(const ::artm::TopicModel& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void RequestTopicModel(const ::artm::core::String& request,
                       ::artm::TopicModel* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void RequestTopicModel(const ::artm::core::String& request,
                       ::artm::TopicModel* response,
                       long deadline_ms = -1);
  void RequestThetaMatrix(const ::artm::core::String& request,
                       ::artm::ThetaMatrix* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void RequestThetaMatrix(const ::artm::core::String& request,
                       ::artm::ThetaMatrix* response,
                       long deadline_ms = -1);
  void RequestScore(const ::artm::core::RequestScoreArgs& request,
                       ::artm::ScoreData* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void RequestScore(const ::artm::core::RequestScoreArgs& request,
                       ::artm::ScoreData* response,
                       long deadline_ms = -1);
  void AddBatch(const ::artm::Batch& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void AddBatch(const ::artm::Batch& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void InvokeIteration(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void InvokeIteration(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void WaitIdle(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void WaitIdle(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
  void InvokePhiRegularizers(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       ::rpcz::rpc* rpc,                     ::rpcz::closure* done);
  void InvokePhiRegularizers(const ::artm::core::Void& request,
                       ::artm::core::Void* response,
                       long deadline_ms = -1);
 private:
  ::rpcz::rpc_channel* channel_;
  ::std::string service_name_;
  bool owns_channel_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(NodeControllerService_Stub);
};


}  // namespace core
}  // namespace artm
#endif  // RPCZ_artm_2fcore_2finternals_2eproto__INCLUDED
