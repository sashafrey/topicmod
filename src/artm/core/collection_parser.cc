// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/collection_parser.h"

#include <sstream>
#include <vector>
#include <string>
#include <map>

#include "boost/lexical_cast.hpp"
#include "boost/iostreams/device/mapped_file.hpp"
#include "boost/iostreams/stream.hpp"

#include "glog/logging.h"

#include "artm/core/exceptions.h"
#include "artm/core/helpers.h"

using boost::iostreams::mapped_file_source;

namespace artm {
namespace core {

CollectionParser::CollectionParser(const ::artm::CollectionParserConfig& config)
    : config_(config) {}

std::shared_ptr<DictionaryConfig> CollectionParser::JustLoadDictionary() {
  auto retval = std::make_shared<DictionaryConfig>();
  ::artm::core::BatchHelpers::LoadMessage(config_.dictionary_file_name(),
                                          config_.target_folder(), retval.get());
  return retval;
}

std::shared_ptr<DictionaryConfig> CollectionParser::ParseBagOfWordsUci() {
  boost::iostreams::stream<mapped_file_source> vocab(config_.vocab_file_path());
  boost::iostreams::stream<mapped_file_source> docword(config_.docword_file_path());

  int num_docs, num_unique_tokens, num_tokens;
  docword >> num_docs >> num_unique_tokens >> num_tokens;

  if (num_docs <= 0)
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("CollectionParser.num_docs", num_docs));

  if (num_unique_tokens <= 0)
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("CollectionParser.num_docs", num_unique_tokens));

  if (num_tokens <= 0)
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("CollectionParser.num_docs", num_tokens));

  std::vector<std::string> dictionary;
  dictionary.reserve(num_unique_tokens);
  auto retval = std::make_shared<DictionaryConfig>();

  for (std::string token; vocab >> token;) {
    if (dictionary.size() == num_unique_tokens) {
      std::stringstream ss;
      ss << config_.vocab_file_path() << " contains more than " << num_tokens
         << " tokens (which is the number of tokens specified in " << config_.docword_file_path()
         << "). The remaining part of vocab file will be ignored. Last non-ignored word was "
         << dictionary.back();
      LOG(ERROR) << ss.str();
      break;
    }

    dictionary.push_back(token);
    artm::DictionaryEntry* entry = retval->add_entry();
    entry->set_key_token(token);
  }

  std::map<int, int> batch_dictionary;
  ::artm::Batch batch;
  ::artm::Item* item;
  ::artm::Field* field;
  int prev_item_id = -1;

  int item_id, token_id, token_count;
  for (std::string token; docword >> item_id >> token_id >> token_count;) {
    token_id--;  // convert 1-based to zero-based index
    if (token_id >= static_cast<int>(dictionary.size())) {
      std::stringstream ss;
      ss << "Unable to interpret line '"
         << item_id << " " << (token_id+1) << " " << token_count << "' in "
         << config_.docword_file_path() << ". Token_id exceeds the dictionary size.";
      LOG(ERROR) << ss.str();
      continue;
    }

    if (item_id != prev_item_id) {
      prev_item_id = item_id;
      if (batch.item_size() >= config_.num_items_per_batch()) {
        ::artm::core::BatchHelpers::SaveBatch(batch, config_.target_folder());
        batch.Clear();
        batch_dictionary.clear();
      }

      item = batch.add_item();
      item->set_id(item_id);
      field = item->add_field();
    }

    auto iter = batch_dictionary.find(token_id);
    if (iter == batch_dictionary.end()) {
      batch_dictionary.insert(std::make_pair(token_id, batch_dictionary.size()));
      batch.add_token(dictionary[token_id]);
      iter = batch_dictionary.find(token_id);
    }

    field->add_token_id(iter->second);
    field->add_token_count(token_count);
  }

  if (batch.item_size() > 0) {
    ::artm::core::BatchHelpers::SaveBatch(batch, config_.target_folder());
  }

  ::artm::core::BatchHelpers::SaveMessage(config_.dictionary_file_name(),
                                          config_.target_folder(), *retval);

  return retval;
}

std::shared_ptr<DictionaryConfig> CollectionParser::Parse() {
  switch (config_.format()) {
    case CollectionParserConfig_Format_BagOfWordsUci:
      return ParseBagOfWordsUci();

    case CollectionParserConfig_Format_JustLoadDictionary:
      return JustLoadDictionary();

    default:
      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(
        "CollectionParserConfig.format", config_.format()));
  }
}

}  // namespace core
}  // namespace artm
