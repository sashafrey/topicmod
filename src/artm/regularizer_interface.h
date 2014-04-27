// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_REGULARIZER_INTERFACE_H_
#define SRC_ARTM_REGULARIZER_INTERFACE_H_

#include <vector>

#include "artm/messages.pb.h"

namespace artm {
namespace core {

class RegularizerInterface {
 public:
  virtual bool RegularizeTheta(const Item& item,
                                std::vector<float> n_dt,
                                int topic_size,
                                int inner_iter) { return false; }
  virtual void RegularizePhi() {}
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_REGULARIZER_INTERFACE_H_
