// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_REGULARIZER_INTERFACE_H_
#define SRC_ARTM_REGULARIZER_INTERFACE_H_

#include "artm/messages.pb.h"

namespace artm {
namespace core {

class RegularizerInterface {
public:
  virtual void RegularizeTheta(const Item& item, float* n_dt, int topic_size, int inner_iter) {}
  virtual void RegularizePhi() {}
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_REGULARIZER_INTERFACE_H_