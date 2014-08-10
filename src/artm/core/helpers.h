// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_HELPERS_H_
#define SRC_ARTM_CORE_HELPERS_H_

#include <memory>
#include <string>
#include <vector>

#include "boost/uuid/uuid.hpp"             // uuid class
#include "boost/filesystem.hpp"

namespace artm {

class Batch;

namespace core {

class Helpers {
 public:
  // Usage: SetThreadName (-1, "MainThread");
  // (thread_id == -1 stands for the current thread)
  static void SetThreadName(int thread_id, const char* thread_name);
};

class BatchHelpers {
 public:
  static void CompactBatch(const Batch& batch, Batch* compacted_batch);
  static std::vector<boost::uuids::uuid> ListAllBatches(const boost::filesystem::path& root);
  static std::shared_ptr<const Batch> LoadBatch(const boost::uuids::uuid& uuid,
                                                const std::string& disk_path);
  static boost::uuids::uuid SaveBatch(const Batch& batch, const std::string& disk_path);
  static std::string MakeBatchPath(const std::string& disk_path, const boost::uuids::uuid& uuid);
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_HELPERS_H_
