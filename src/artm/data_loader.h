// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_DATA_LOADER_H_
#define SRC_ARTM_DATA_LOADER_H_

#include <list>
#include <set>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"
#include "boost/uuid/uuid.hpp"

#include "artm/batch_manager.h"
#include "artm/common.h"
#include "artm/generation.h"
#include "artm/instance.h"
#include "artm/internals.pb.h"
#include "artm/messages.pb.h"
#include "artm/template_manager.h"
#include "artm/thread_safe_holder.h"

namespace artm {
namespace core {

class DataLoader : boost::noncopyable {
 public:
  ~DataLoader();

  int GetTotalItemsCount() const;
  void AddBatch(const Batch& batch);
  void Callback(std::shared_ptr<const ProcessorOutput> cache);
  void Reconfigure(const DataLoaderConfig& config);
  void Interrupt();
  void InvokeIteration(int iterations_count);
  void Join();
  void WaitIdle();
  void DisposeModel(ModelName model_name);
  bool RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix);

  int id() const;

 private:
  friend class TemplateManager<DataLoader, DataLoaderConfig>;

  // All instances of DataLoader should be created via DataLoaderManager
  DataLoader(int id, const DataLoaderConfig& config);
  static void CompactBatch(const Batch& batch, Batch* compacted_batch);

  int data_loader_id_;

  boost::mutex lock_;
  ThreadSafeHolder<DataLoaderConfig> config_;
  ThreadSafeHolder<Generation> generation_;

  typedef std::pair<boost::uuids::uuid, ModelName> CacheKey;
  boost::mutex cache_lock_;
  ThreadSafeCollectionHolder<CacheKey, DataLoaderCacheEntry> cache_;

  boost::mutex batch_manager_lock_;
  BatchManager batch_manager_;

  // Keep all threads at the end of class members
  // (because the order of class members defines initialization order;
  // everything else should be initialized before creating threads).
  boost::thread thread_;

  void ThreadFunction();
};

typedef TemplateManager<DataLoader, DataLoaderConfig> DataLoaderManager;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_DATA_LOADER_H_
