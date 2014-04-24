// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/generation.h"

namespace artm {
namespace core {

int Generation::id() const {
  return id_;
}

std::shared_ptr<const Batch> Generation::batch(const boost::uuids::uuid& uuid) {
  auto retval = generation_.find(uuid);
  return (retval != generation_.end()) ? retval->second : nullptr;
}

void Generation::AddBatch(const std::shared_ptr<const Batch>& batch) {
  generation_.insert(std::make_pair(boost::uuids::random_generator()(), batch));
}

int Generation::GetTotalItemsCount() const {
  int retval = 0;
  for (auto iter = generation_.begin(); iter != generation_.end(); ++iter) {
    retval += (*iter).second->item_size();
  }

  return retval;
}

}  // namespace core
}  // namespace artm

