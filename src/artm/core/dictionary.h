// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_DICTIONARY_H_
#define SRC_ARTM_CORE_DICTIONARY_H_

#include <string>
#include <map>
#include "artm\messages.pb.h"

namespace artm {
namespace core {

typedef std::map<std::string, DictionaryEntry> DictionaryMap;

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_DICTIONARY_H_
