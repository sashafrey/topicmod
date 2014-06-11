// Copyright 2014, Additive Regularization of Topic Models.

// Author: Murat Apishev (great-mel@yandex.ru)

#ifndef SRC_ARTM_REGULARIZER_INTERFACE_H_
#define SRC_ARTM_REGULARIZER_INTERFACE_H_

#include <vector>
#include <map>
#include <memory>

#include "artm/messages.pb.h"
#include "artm/topic_model.h"

namespace artm {
namespace core {

class RegularizerInterface {
 public:
  virtual ~RegularizerInterface() { }

  virtual bool RegularizeTheta(const Item& item,
                               std::vector<float>* n_dt,
                               int topic_size,
                               int inner_iter,
                               double tau) { return true; }

  virtual bool RegularizePhi(TopicModel* topic_model, double tau, 
    std::vector<std::pair<std::string, 
    std::shared_ptr<std::map<std::string, DictionaryEntry>> >> 
    dictionaries) { return true; }

  virtual RegularizerInternalState RetrieveInternalState() { 
    RegularizerInternalState temp_state;
    return temp_state;
  }

  ::google::protobuf::RepeatedPtrField<std::string> dictionary_name() { return dictionary_name_; }

  void set_dictionary_name(::google::protobuf::RepeatedPtrField<std::string>& dictionary_name) {
    dictionary_name_.CopyFrom(dictionary_name);
  }

 private:
  ::google::protobuf::RepeatedPtrField<std::string> dictionary_name_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_REGULARIZER_INTERFACE_H_
