// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_SMOOTH_SPARSE_PHI_H_
#define SRC_ARTM_SMOOTH_SPARSE_PHI_H_

#include <vector>

#include "artm/messages.pb.h"
#include "artm/regularizer_interface.h"

namespace artm {
namespace core {
namespace regularizer {

class SmoothSparsePhi : public RegularizerInterface {
 public:
  explicit SmoothSparsePhi(const SmoothSparsePhiConfig& config)
    : config_(config) {}

  virtual bool RegularizePhi(TopicModel* topic_model);

 private:
  SmoothSparsePhiConfig config_;
};

}  // namespace regularizer
}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_SMOOTH_SPARSE_PHI_H_
