// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_BATCH_MANAGER_H_
#define SRC_ARTM_BATCH_MANAGER_H_

#include <list>
#include <set>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"
#include "boost/uuid/uuid.hpp"

#include "artm/common.h"

namespace artm {
namespace core {

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

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_BATCH_MANAGER_H_
