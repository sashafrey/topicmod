// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_
#define SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_

#include <vector>

#include "artm/messages.pb.h"
#include "artm/regularizer_interface.h"

namespace artm {
namespace core {

class DirichletRegularizerTheta : public RegularizerInterface {
 public:
  explicit DirichletRegularizerTheta(const DirichletRegularizerThetaConfig& config)
    : config_(config) {}

  virtual bool RegularizeTheta(const Item& item,
                               std::vector<float>* n_dt,
                               int topic_size,
                               int inner_iter);

 private:
  DirichletRegularizerThetaConfig config_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_
