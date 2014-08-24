// Copyright 2014, Additive Regularization of Topic Models.

#include "boost/filesystem.hpp"

#include "gtest/gtest.h"

#include "artm/messages.pb.h"
#include "artm/cpp_interface.h"

// To run this particular test:
// artm_tests.exe --gtest_filter=CollectionParser.*
TEST(CollectionParser, Basic) {
  // Clean all .batches files
  if (boost::filesystem::exists("collection_parser_test")) {
    boost::filesystem::recursive_directory_iterator it("collection_parser_test");
    boost::filesystem::recursive_directory_iterator endit;
    while (it != endit) {
      if (boost::filesystem::is_regular_file(*it)) {
        if (it->path().extension() == ".batch" || it->path().extension() == ".dictionary")
          boost::filesystem::remove(*it);
      }

      ++it;
    }
  }

  ::artm::CollectionParserConfig config;
  config.set_format(::artm::CollectionParserConfig_Format_BagOfWordsUci);
  config.set_target_folder("collection_parser_test/");
  config.set_dictionary_file_name("test_parser.dictionary");
  config.set_num_items_per_batch(1);
  config.set_vocab_file_path("../../../test_data/vocab.parser_test.txt");
  config.set_docword_file_path("../../../test_data/docword.parser_test.txt");

  std::shared_ptr<::artm::DictionaryConfig> dictionary_parsed = ::artm::ParseCollection(config);
  ASSERT_EQ(dictionary_parsed->entry_size(), 3);

  config.set_format(::artm::CollectionParserConfig_Format_JustLoadDictionary);
  std::shared_ptr<::artm::DictionaryConfig> dictionary_loaded = ::artm::ParseCollection(config);
  ASSERT_EQ(dictionary_parsed->entry_size(), dictionary_loaded->entry_size());

  boost::filesystem::recursive_directory_iterator it("collection_parser_test");
  boost::filesystem::recursive_directory_iterator endit;
  int batches_count = 0;
  while (it != endit) {
    if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ".batch")
      batches_count++;
    ++it;
  }

  ASSERT_EQ(batches_count, 2);
}

TEST(CollectionParser, ErrorHandling) {
  ::artm::CollectionParserConfig config;
  config.set_format(::artm::CollectionParserConfig_Format_BagOfWordsUci);
  config.set_vocab_file_path("no_such_file.txt");
  config.set_docword_file_path("../../../test_data/docword.parser_test.txt");
  ASSERT_THROW(::artm::ParseCollection(config), artm::DiskReadException);

  config.set_vocab_file_path("../../../test_data/vocab.parser_test.txt");
  config.set_docword_file_path("no_such_file");
  ASSERT_THROW(::artm::ParseCollection(config), artm::DiskReadException);

  config.set_format(::artm::CollectionParserConfig_Format_JustLoadDictionary);
  config.set_target_folder("no_such_folder");
  config.set_dictionary_file_name("no_such_file.dictionary");
  ASSERT_THROW(::artm::ParseCollection(config), artm::DiskReadException);
}
