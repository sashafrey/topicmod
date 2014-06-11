// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_REGULARIZERS_SANDBOX_DIRICHLET_PHI_H_
#define SRC_ARTM_REGULARIZERS_SANDBOX_DIRICHLET_PHI_H_

#include <vector>

#include "artm/messages.pb.h"
#include "artm/regularizer_interface.h"

namespace artm {
namespace core {
namespace regularizer {

class DirichletPhi : public RegularizerInterface {
 public:

  virtual bool RegularizePhi(TopicModel* topic_model, double tau, 
    std::vector<std::pair<std::string, 
    std::shared_ptr<std::map<std::string, DictionaryEntry>> >> 
    dictionaries);

};

}  // namespace regularizer
}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_REGULARIZERS_SANDBOX_DIRICHLET_PHI_H_
