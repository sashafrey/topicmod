// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#include <vector>

#include "artm/regularizers_sandbox/dirichlet_theta.h"

namespace artm {
namespace core {
namespace regularizer {

bool DirichletTheta::RegularizeTheta(const Item& item,
                                     std::vector<float>* n_dt,
                                     int topic_size,
                                     int inner_iter,
                                     double tau) {
  const ::google::protobuf::RepeatedPtrField<DoubleArray>& alpha_vector =
    config_.alpha();

  // inner_iter is from [0 iter_num]
  if (alpha_vector.size() < inner_iter + 1) {
    for (int i = 0; i < topic_size; ++i) {
      (*n_dt)[i] = (*n_dt)[i] + static_cast<float>(tau * 1);
    }   
  } else {
    const artm::DoubleArray& alpha = alpha_vector.Get(inner_iter);
    if (alpha.value_size() == topic_size) {
      for (int i = 0; i < topic_size; ++i) {
        (*n_dt)[i] = (*n_dt)[i] + static_cast<float>(tau * alpha.value().Get(i));
      }
    } else {
      return false;
    }
  }
  return true; 
}

}  // namespace regularizer
}  // namespace core
}  // namespace artm
