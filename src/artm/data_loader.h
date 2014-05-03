// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_DATA_LOADER_H_
#define SRC_ARTM_DATA_LOADER_H_

#include <list>
#include <set>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"
#include "boost/uuid/uuid.hpp"

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

  int id() const;

 private:
  // Each batch should be processed by at max one processor at a time.
  // Consider a scenario when there is a very slow processor,
  // and it keeps processing the batch when DataLoader starts the next iteration.
  // In such situation BatchManager will ensure that no other processors will receive the
  // batch until slow processor is done.
  class BatchManager : boost::noncopyable {
   public:
    explicit BatchManager(boost::mutex* lock);

    // Add batch to the task queue.
    // OK to add the same uuid multiple times.
    void Add(const boost::uuids::uuid& id);

    // Select next available batch, and excludes it from task queue.
    // This operation skips all "in progress" batches.
    // The batch return by this operation will stay in "in progress" list
    // until it is marked as processed by Done().
    boost::uuids::uuid Next();

    // Eliminates uuid from "in progress" set.
    void Done(const boost::uuids::uuid& id);

    // Checks if all added tasks were processed (and marked as "Done").
    bool IsEverythingProcessed() const;

   private:
    mutable boost::mutex* lock_;
    std::list<boost::uuids::uuid> tasks_;
    std::set<boost::uuids::uuid> in_progress_;
  };

  friend class TemplateManager<DataLoader, DataLoaderConfig>;

  // All instances of DataLoader should be created via DataLoaderManager
  DataLoader(int id, const DataLoaderConfig& config);
  static void CompactBatch(const Batch& batch, Batch* compacted_batch);

  int data_loader_id_;

  boost::mutex lock_;
  ThreadSafeHolder<DataLoaderConfig> config_;
  ThreadSafeHolder<Generation> generation_;

  boost::mutex cache_lock_;
  ThreadSafeCollectionHolder<boost::uuids::uuid, const ProcessorOutput> cache_;

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
