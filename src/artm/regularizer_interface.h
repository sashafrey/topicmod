// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_REGULARIZER_INTERFACE_H_
#define SRC_ARTM_REGULARIZER_INTERFACE_H_

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "artm/messages.pb.h"

namespace artm {

namespace core {
  // Forward declarations
  class TopicModel;
  template<typename K, typename T> class ThreadSafeCollectionHolder;
  typedef std::map<std::string, ::artm::DictionaryEntry> DictionaryMap;
  typedef ThreadSafeCollectionHolder<std::string, DictionaryMap> ThreadSafeDictionaryCollection;
}

class RegularizerInterface {
 public:
  RegularizerInterface() : dictionaries_(nullptr) {}
  virtual ~RegularizerInterface() { }

  virtual bool RegularizeTheta(const Item& item,
                               std::vector<float>* n_dt,
                               int topic_size,
                               int inner_iter,
                               double tau) { return true; }

  virtual bool RegularizePhi(::artm::core::TopicModel* topic_model, double tau) { return true; }

  std::shared_ptr<::artm::core::DictionaryMap> dictionary(const std::string& dictionary_name);
  void set_dictionaries(const ::artm::core::ThreadSafeDictionaryCollection* dictionaries);

 private:
  const ::artm::core::ThreadSafeDictionaryCollection* dictionaries_;
};

}  // namespace artm

#endif  // SRC_ARTM_REGULARIZER_INTERFACE_H_
