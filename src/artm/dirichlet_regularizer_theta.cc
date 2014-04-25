// Copyright 2014, Additive Regularization of Topic Models.

#include <iostream>

#include "artm/dirichlet_regularizer_theta.h"

namespace artm {
namespace core {

DirichletRegularizerTheta::DirichletRegularizerTheta(DirichletRegularizerThetaConfig config) 
  : config_(config) {}

void DirichletRegularizerTheta::RegularizeTheta(const Item& item, float* n_dt, 
                                               int topic_size, int inner_iter) {
  //const ::google::protobuf::RepeatedField<double>& alpha_0 = config_.alpha_0();
  //if (alpha_0.size >= inner_iter) {
  //  alpha_0.Get(inner_iter)
  //    // check the 0-1 indices --- what is the start of inner_iter in InferTheta!
  //} else {
  //  std::cout << "Warning: not enough input parameters for regularizer" << name_
  //    << "! This regularizer was turned off on current iteration!" << std::endl;
  // move this to InferTheta and make switch on all warning messages!
  //}
}

}  // namespace core
}  // namespace artm