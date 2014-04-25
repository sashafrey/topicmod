// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_REGULARIZER_INTERFACE_H_
#define SRC_ARTM_REGULARIZER_INTERFACE_H_

#define REGULARIZATION_SUCCESS 0
#define REGULARIZATION_FAILED -1

#include <vector>

#include "boost/thread/mutex.hpp"

#include "artm/messages.pb.h"

namespace artm {
namespace core {

class RegularizerInterface {
public:
  virtual void RegularizeTheta(const Item& item, 
                                std::vector<float> n_dt, 
                                int topic_size, 
                                int inner_iter, 
                                std::shared_ptr<int> retval) {}
  virtual void RegularizePhi() {}

};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_REGULARIZER_INTERFACE_H_