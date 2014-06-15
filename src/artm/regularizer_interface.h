// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_REGULARIZER_INTERFACE_H_
#define SRC_ARTM_REGULARIZER_INTERFACE_H_

#include <vector>
#include <map>
#include <memory>

#include "artm/messages.pb.h"
#include "artm/core/dictionary.h"
#include "artm/core/thread_safe_holder.h"
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

  virtual bool RegularizePhi(core::TopicModel* topic_model, double tau) { return true; }

  virtual RegularizerInternalState RetrieveInternalState() { 
    RegularizerInternalState temp_state;
    return temp_state;
  }

  std::shared_ptr<core::DictionaryMap> dictionary(std::string dictionary_name) {
    return dictionaries_->get(dictionary_name);
  }

  void set_dictionaries(const core::ThreadSafeCollectionHolder<std::string, core::DictionaryMap>* 
    dictionaries) {
    dictionaries_ = dictionaries;
  }

 private:
  const core::ThreadSafeCollectionHolder<std::string, core::DictionaryMap>* dictionaries_;
};

}  // namespace artm

#endif  // SRC_ARTM_REGULARIZER_INTERFACE_H_
