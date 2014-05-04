// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/dirichlet_regularizer_theta.h"

namespace artm {
namespace core {

bool DirichletRegularizerTheta::RegularizeTheta(const Item& item,
                                                std::vector<float>* n_dt,
                                                int topic_size,
                                                int inner_iter) {
  const ::google::protobuf::RepeatedField<double>& alpha_0_vector = config_.alpha_0();
  const ::google::protobuf::RepeatedPtrField<DoubleArray>& tilde_alpha_vector =
    config_.tilde_alpha();

  // inner_iter is from [0 iter_num]
  if ((alpha_0_vector.size() >= inner_iter + 1) && (tilde_alpha_vector.size() >= inner_iter + 1)) {
    double alpha_0 = alpha_0_vector.Get(inner_iter);
    const artm::DoubleArray& tilde_alpha = tilde_alpha_vector.Get(inner_iter);

    if (tilde_alpha.elem_size() == topic_size) {
      for (int i = 0; i < topic_size; ++i) {
        (*n_dt)[i] = (*n_dt)[i] + static_cast<float>(alpha_0 * tilde_alpha.elem().Get(i));
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

}  // namespace core
}  // namespace artm
