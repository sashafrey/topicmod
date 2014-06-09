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
                                     int inner_iter,
                                     double tau) {
  // read the parameters from config
  const int background_topics_count = config_.background_topics_count();
  const ::google::protobuf::RepeatedPtrField<DoubleArray>& alpha_vector =
  config_.alpha();

  // control the correctness of parametres
  if (background_topics_count < 0 || background_topics_count > topic_size) return false;

  if (alpha_vector.size() < inner_iter + 1) {
    // proceed the regularization
    for (int i = 0; i < topic_size; ++i) {
      (*n_dt)[i] = (*n_dt)[i] + static_cast<float>(tau * 1);
    }
  } else {
    const artm::DoubleArray& alpha = alpha_vector.Get(inner_iter);
    if (alpha.value_size() != topic_size) return false;

    // proceed the regularization
    for (int i = 0; i < topic_size; ++i) {
      (*n_dt)[i] = (*n_dt)[i] + static_cast<float>(tau * alpha.value().Get(i));
    }
  }
  return true;
}

}  // namespace regularizer
}  // namespace core
}  // namespace artm
