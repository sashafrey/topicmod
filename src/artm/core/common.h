// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_COMMON_H_
#define SRC_ARTM_CORE_COMMON_H_

#include <memory>
#include <string>

#include "internals.pb.h"

namespace artm {
namespace core {

typedef std::string ModelName;

const int UnknownId = -1;

const std::string kBatchExtension = ".batch";

class Notifiable {
public:
 virtual ~Notifiable() {};
 virtual void Callback(std::shared_ptr<const ModelIncrement> model_increment) = 0;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_COMMON_H_
