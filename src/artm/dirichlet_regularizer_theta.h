// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_
#define SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_

#include "artm/messages.pb.h"
#include "artm/regularizer_interface.h"

namespace artm {
namespace core {

class DirichletRegularizerTheta : RegularizerInterface {
public:
  DirichletRegularizerTheta(const std::string& config);
  ~DirichletRegularizerTheta();

  void RegularizeTheta(const Item& item, float* n_dt, int topic_size, int inner_iter);

private:
  float* tilde_alpha_;
  float* alpha_0;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_DIRICHLET_REGULARIZER_THETA_H_