// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_REGULARIZER_INTERFACE_H_
#define SRC_ARTM_REGULARIZER_INTERFACE_H_

#include <vector>

#include "artm/messages.pb.h"
#include "artm/core/topic_model.h"

namespace artm {

class RegularizerInterface {
 public:
  virtual ~RegularizerInterface() { }

  virtual bool RegularizeTheta(const Item& item,
                                std::vector<float>* n_dt,
                                int topic_size,
                                int inner_iter,
                                double tau) { return true; }

  virtual bool RegularizePhi(::artm::core::TopicModel* topic_model, double tau) { return true; }
};

}  // namespace artm

#endif  // SRC_ARTM_REGULARIZER_INTERFACE_H_
