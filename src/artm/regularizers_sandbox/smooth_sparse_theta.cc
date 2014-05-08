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
    return true;
}

}  // namespace regularizer
}  // namespace core
}  // namespace artm
