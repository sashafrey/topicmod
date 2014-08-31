// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_COLLECTION_PARSER_H_
#define SRC_ARTM_CORE_COLLECTION_PARSER_H_

#include <memory>

#include "boost/utility.hpp"

#include "artm/messages.pb.h"

namespace artm {
namespace core {

class CollectionParser : boost::noncopyable {
 public:
  explicit CollectionParser(const ::artm::CollectionParserConfig& config);

  // Parses the collection from disk according to all options,
  // specified in CollectionParserConfig.
  // Returns a dictionary that lists all unique tokens occured in the collection.
  // Each DictionaryEntry from dictionary will contain key_token,
  // and no other fields in the entry will be populated.
  std::shared_ptr<DictionaryConfig> Parse();

 private:
  std::shared_ptr<DictionaryConfig> JustLoadDictionary();
  std::shared_ptr<DictionaryConfig> ParseBagOfWordsUci();

  CollectionParserConfig config_;
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_COLLECTION_PARSER_H_
