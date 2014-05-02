// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_GENERATION_H_
#define SRC_ARTM_GENERATION_H_

#include <map>
#include <set>
#include <memory>
#include <string>

#include "boost/uuid/uuid.hpp"             // uuid class
#include "boost/uuid/uuid_generators.hpp"  // generators
#include <boost/filesystem.hpp>

#include "artm/messages.pb.h"

namespace artm {
namespace core {

class Generation {
 public:

  Generation(const std::string& disk_path);

  Generation(const Generation& generation)
      : id_(generation.id_ + 1), generation_(generation.generation_) {}

  int id() const;
  std::shared_ptr<const Batch> batch(const boost::uuids::uuid& uuid,
                                     const std::string& disk_path);
  void Generation::AddBatch(const std::shared_ptr<const Batch>& batch,
                            const std::string& disk_path);
  static std::vector<boost::filesystem::path> GetAll(const boost::filesystem::path& root,
                                                     const std::string& ext);
  static std::string MakeBatchPath(std::string disk_path, boost::uuids::uuid uuid);
  int GetTotalItemsCount() const;

  template<class Function>
  void InvokeOnEachPartition(Function fn) const {
    for (auto iter = generation_.begin(); iter != generation_.end(); ++iter) {
      fn(iter->first, iter->second);
    }
  }


 private:
  int id_;
  std::map<boost::uuids::uuid, std::shared_ptr<const Batch> > generation_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_GENERATION_H_
