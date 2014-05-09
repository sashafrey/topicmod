// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/regularizers_sandbox/smooth_sparse_theta.h"

namespace artm {
namespace core {
namespace regularizer {

bool SmoothSparseTheta::RegularizeTheta(const Item& item,
                                     std::vector<float>* n_dt,
                                     int topic_size,
                                     int inner_iter) {
  // read the parameters from config
  const ::google::protobuf::RepeatedField<double>& alpha_0_vector = config_.alpha_0();
  const ::google::protobuf::RepeatedPtrField<DoubleArray>& tilde_alpha_vector =
  config_.tilde_alpha();

  // control the correctness of parametres
  if (alpha_0_vector.size() < inner_iter + 1 ||
      tilde_alpha_vector.size() < inner_iter + 1) return false;
  const artm::DoubleArray& tilde_alpha = tilde_alpha_vector.Get(inner_iter);
  if (tilde_alpha.value_size() != topic_size) return false;

  // proceed the regularization
  double alpha_0 = alpha_0_vector.Get(inner_iter);
  for (int i = 0; i < topic_size; ++i) {
    (*n_dt)[i] = (*n_dt)[i] + static_cast<float>(alpha_0 * tilde_alpha.value().Get(i));
  }
  return true;
}

}  // namespace regularizer
}  // namespace core
}  // namespace artm
