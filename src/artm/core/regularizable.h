// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_REGULARIZABLE_H_
#define SRC_ARTM_CORE_REGULARIZABLE_H_


#include <string>

#include "artm/messages.pb.h"

#include "artm/core/common.h"
#include "artm/core/internals.pb.h"

namespace artm {
namespace core {

class TopicWeightIterator;

class Regularizable {
 public:
  virtual void IncreaseRegularizerWeight(const std::string& token, int topic_id, float value) = 0;
  virtual void IncreaseRegularizerWeight(int token_id, int topic_id, float value) = 0;

  virtual int token_size() const = 0;
  virtual int topic_size() const = 0;
  virtual std::string token(int index) const = 0;

  TopicWeightIterator GetTopicWeightIterator(const std::string& token) const;
  TopicWeightIterator GetTopicWeightIterator(int token_id) const;

};


}  // namespace core
}  // namespace artm


#endif  // SRC_ARTM_CORE_REGULARIZABLE_H_
