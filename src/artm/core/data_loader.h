// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_DATA_LOADER_H_
#define SRC_ARTM_CORE_DATA_LOADER_H_

#include <atomic>
#include <list>
#include <set>
#include <utility>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"
#include "boost/uuid/uuid.hpp"

#include "artm/messages.pb.h"
#include "artm/internals.pb.h"
#include "artm/core/batch_manager.h"
#include "artm/core/common.h"
#include "artm/core/template_manager.h"
#include "artm/core/thread_safe_holder.h"

namespace rpcz {
  class application;
}

namespace artm {
namespace core {

class MasterComponentService_Stub;
class Generation;

class DataLoader : boost::noncopyable {
 public:
  DataLoader(int id, const DataLoaderConfig& config);
  virtual ~DataLoader() {}

  int id() const;
  static void PopulateDataStreams(const DataLoaderConfig& config, const Batch& batch,
                                  ProcessorInput* pi);

  virtual void Callback(std::shared_ptr<const ProcessorOutput> cache) = 0;
  virtual void Reconfigure(const DataLoaderConfig& config);

 protected:
  boost::mutex lock_;
  ThreadSafeHolder<DataLoaderConfig> config_;

 private:
  int data_loader_id_;
};

// DataLoader for local modus operandi
class LocalDataLoader : public DataLoader {
 public:
  virtual ~LocalDataLoader();

  int GetTotalItemsCount() const;
  void AddBatch(const Batch& batch);
  virtual void Callback(std::shared_ptr<const ProcessorOutput> cache);

  void InvokeIteration(int iterations_count);
  void WaitIdle();
  void DisposeModel(ModelName model_name);
  bool RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix);

 private:
  friend class TemplateManager<DataLoader, DataLoaderConfig>;

  // All instances of DataLoader should be created via DataLoaderManager
  LocalDataLoader(int id, const DataLoaderConfig& config);
  static void CompactBatch(const Batch& batch, Batch* compacted_batch);

  ThreadSafeHolder<Generation> generation_;

  typedef std::pair<boost::uuids::uuid, ModelName> CacheKey;
  boost::mutex cache_lock_;
  ThreadSafeCollectionHolder<CacheKey, DataLoaderCacheEntry> cache_;

  boost::mutex batch_manager_lock_;
  BatchManager batch_manager_;

  mutable std::atomic<bool> is_stopping;

  // Keep all threads at the end of class members
  // (because the order of class members defines initialization order;
  // everything else should be initialized before creating threads).
  boost::thread thread_;

  void ThreadFunction();
};

// DataLoader for network modus operandi
class RemoteDataLoader : public DataLoader {
 public:
  virtual ~RemoteDataLoader();
  virtual void Reconfigure(const DataLoaderConfig& config);
  virtual void Callback(std::shared_ptr<const ProcessorOutput> cache);

 private:
  friend class TemplateManager<DataLoader, DataLoaderConfig>;

  // All instances of DataLoader should be created via NetworkLoaderManager
  RemoteDataLoader(int id, const DataLoaderConfig& config);

  std::unique_ptr<rpcz::application> application_;
  std::shared_ptr<artm::core::MasterComponentService_Stub> master_component_service_proxy_;

  mutable std::atomic<bool> is_stopping;

  // Keep all threads at the end of class members
  // (because the order of class members defines initialization order;
  // everything else should be initialized before creating threads).
  boost::thread thread_;

  void ThreadFunction();
};

typedef TemplateManager<DataLoader, DataLoaderConfig> DataLoaderManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_DATA_LOADER_H_
