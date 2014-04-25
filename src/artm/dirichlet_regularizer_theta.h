// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_
#define SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_

#include "artm/messages.pb.h"
#include "artm/regularizer_interface.h"

namespace artm {
namespace core {

class DirichletRegularizerTheta : RegularizerInterface {
public:
  DirichletRegularizerTheta(DirichletRegularizerThetaConfig config);

  void RegularizeTheta(const Item& item, float* n_dt, int topic_size, int inner_iter);

private:
  DirichletRegularizerThetaConfig config_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_