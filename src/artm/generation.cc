// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/generation.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace artm {
namespace core {


// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
std::vector<boost::filesystem::path> GetAll(const boost::filesystem::path& root, 
                                            const std::string& ext)
{
  std::vector<boost::filesystem::path> filesList;

  if (boost::filesystem::exists(root) && boost::filesystem::is_directory(root)) {
    boost::filesystem::recursive_directory_iterator it(root);
    boost::filesystem::recursive_directory_iterator endit;
    while(it != endit) {
      if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ext) {
        filesList.push_back(it->path().filename());
      }
      ++it;
    }
  }
  return filesList;
}

Generation::Generation(const std::string disk_path) : id_(0), generation_() {
  std::string batchExtension = ".batch";
  std::vector<boost::filesystem::path> batchFiles = GetAll(disk_path, batchExtension);
  for (size_t i = 0; i < batchFiles.size(); ++i) {
    std::string uuid_str = batchFiles[i].stem().string();
    boost::uuids::string_generator gen_str;
    generation_.insert(std::make_pair(gen_str(uuid_str), nullptr));
  }
}

int Generation::id() const {
  return id_;
}

std::shared_ptr<const Batch> Generation::batch(const boost::uuids::uuid& uuid) {
  auto retval = generation_.find(uuid);
  return (retval != generation_.end()) ? retval->second : nullptr;
}

std::shared_ptr<const Batch> Generation::batch(const boost::uuids::uuid& uuid, const std::string disk_path) {
  auto retval = generation_.find(uuid);
  std::shared_ptr<Batch> batch_ptr = nullptr;
  if (retval != generation_.end()) {
    std::string batch_file = disk_path + '\\' + boost::lexical_cast<std::string>(uuid) + ".batch";
    std::ifstream fin(batch_file.c_str(), std::ifstream::binary);
    if (fin.is_open()) {
      Batch batch;
      bool is_parsed = batch.ParseFromIstream(&fin);
      if (is_parsed) {
        batch_ptr = std::make_shared<Batch>(batch);;
      }
      fin.close();
    }
  }
  return batch_ptr;
}

void Generation::AddBatch(const std::shared_ptr<const Batch>& batch) {
  generation_.insert(std::make_pair(boost::uuids::random_generator()(), batch));
}

void Generation::AddBatch(const std::shared_ptr<const Batch>& batch, const std::string disk_path) {
  boost::uuids::uuid id = boost::uuids::random_generator()();
  generation_.insert(std::make_pair(id, nullptr));
  std::string batch_file = disk_path + '\\' + boost::lexical_cast<std::string>(id) + ".batch";
  std::ofstream fout(batch_file.c_str(), std::ofstream::binary);
  if (fout.is_open()) {
    bool is_serialized = batch->SerializeToOstream(&fout);
    fout.close();
  }
}

int Generation::GetTotalItemsCount() const {
  int retval = 0;
  for (auto iter = generation_.begin(); iter != generation_.end(); ++iter) {
    if ((*iter).second != nullptr) {
      retval += (*iter).second->item_size();
    }
  }

  return retval;
}

}  // namespace core
}  // namespace artm

