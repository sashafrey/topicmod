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
  if (!boost::filesystem::exists(config_.vocab_file_path()))
    BOOST_THROW_EXCEPTION(DiskReadException("File " + config_.vocab_file_path() + " does not exist."));

  if (!boost::filesystem::exists(config_.docword_file_path()))
    BOOST_THROW_EXCEPTION(DiskReadException("File " + config_.docword_file_path() + " does not exist."));

  boost::iostreams::stream<mapped_file_source> vocab(config_.vocab_file_path());
  boost::iostreams::stream<mapped_file_source> docword(config_.docword_file_path());

  int num_docs, num_unique_tokens, num_tokens;
  docword >> num_docs >> num_unique_tokens >> num_tokens;

  if (num_docs <= 0) {
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(
      "CollectionParser.num_docs (D)", num_docs));
  }

  if (num_unique_tokens <= 0) {
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(
      "CollectionParser.num_unique_tokens (W)", num_unique_tokens));
  }

  if (num_tokens <= 0) {
    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(
      "CollectionParser.num_tokens (NNZ)", num_tokens));
  }

  std::vector<std::string> dictionary;
  dictionary.reserve(num_unique_tokens);
  auto retval = std::make_shared<DictionaryConfig>();

  for (std::string token; vocab >> token;) {
    if (static_cast<int>(dictionary.size()) >= num_unique_tokens) {
      std::stringstream ss;
      ss << config_.vocab_file_path() << " contains too many tokens. "
         << "Expected number is " << num_unique_tokens
         << ", as it is specified in " << config_.docword_file_path()
         << "). Last read word was '" << dictionary.back() << "'.";

      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(
        "CollectionParser.num_unique_tokens (W)", (num_unique_tokens + 1), ss.str()));
    }

    dictionary.push_back(token);
    artm::DictionaryEntry* entry = retval->add_entry();
    entry->set_key_token(token);
  }

  if (static_cast<int>(dictionary.size()) != num_unique_tokens) {
    std::stringstream ss;
    ss << config_.vocab_file_path() << " contains not enough tokens. "
         << "Expected number is " << num_unique_tokens
         << ", as it is specified in " << config_.docword_file_path()
         << "). Last read word was '" << dictionary.back() << "'.";

    BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException(
      "CollectionParser.num_unique_tokens (W)", (dictionary.size() + 1), ss.str()));
  }

  std::map<int, int> batch_dictionary;
  ::artm::Batch batch;
  ::artm::Item* item = nullptr;
  ::artm::Field* field = nullptr;
  int prev_item_id = -1;

  int item_id, token_id, token_count;
  for (std::string token; docword >> item_id >> token_id >> token_count;) {
    if ((token_id <= 0) || (token_id > static_cast<int>(dictionary.size()))) {
      std::stringstream ss;
      ss << "Failed to parse line '" << item_id << " " << token_id << " " << token_count << "' in "
         << config_.docword_file_path();
      if (token_id == 0) {
        ss << ". wordID column appears to be zero-based in the docword file being parsed. "
           << "UCI format defines wordID column to be unity-based. "
           << "Please, increase wordID by one in your input data.";
      } else {
        ss << ". Token_id value is outside of the expected range.";
      }

      BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("wordID", token_id, ss.str()));
    }

    token_id--;  // convert 1-based to zero-based index

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
