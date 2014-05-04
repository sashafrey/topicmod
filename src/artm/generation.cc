// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/generation.h"

#include <fstream>  // NOLINT

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/filesystem.hpp"

#include "glog/logging.h"


namespace artm {
namespace core {

Generation::Generation(const std::string& disk_path) : id_(0), generation_() {
  if (!disk_path.empty()) {
    std::string batchExtension = ".batch";
    std::vector<boost::filesystem::path> batchFiles = GetAll(disk_path, batchExtension);
    for (size_t i = 0; i < batchFiles.size(); ++i) {
      std::string uuid_str = batchFiles[i].stem().string();
      boost::uuids::string_generator gen_str;
      generation_.insert(std::make_pair(gen_str(uuid_str), nullptr));
    }
  }
}

int Generation::id() const {
  return id_;
}

std::shared_ptr<const Batch> Generation::batch(const boost::uuids::uuid& uuid,
                                               const std::string& disk_path) {
  std::shared_ptr<const Batch> batch_ptr = nullptr;
  auto retval = generation_.find(uuid);
  if (retval != generation_.end()) {
    if (disk_path.empty()) {
      batch_ptr = retval->second;
    } else {
      std::string batch_file = MakeBatchPath(disk_path, uuid);
      std::ifstream fin(batch_file.c_str(), std::ifstream::binary);
      if (fin.is_open()) {
        std::shared_ptr<Batch> batch_loaded(new Batch());
        bool is_parsed = batch_loaded->ParseFromIstream(&fin);
        if (is_parsed) {
          batch_ptr = batch_loaded;
        }
        fin.close();
      }
    }
  }
  return batch_ptr;
}

void Generation::AddBatch(const std::shared_ptr<const Batch>& batch,
                          const std::string& disk_path) {
  if (disk_path.empty()) {
    generation_.insert(std::make_pair(boost::uuids::random_generator()(), batch));
  } else {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    generation_.insert(std::make_pair(uuid, nullptr));
    std::string batch_file = MakeBatchPath(disk_path, uuid);
    std::ofstream fout(batch_file.c_str(), std::ofstream::binary);
    if (fout.is_open()) {
      bool is_serialized = batch->SerializeToOstream(&fout);
      if (!is_serialized) {
        LOG(ERROR) << "Batch has not been serialized on disk.";
      }
      fout.close();
    }
  }
}

// Return the filenames of all files that have the specified extension
// in the specified directory.
std::vector<boost::filesystem::path> Generation::GetAll(const boost::filesystem::path& root,
                                                        const std::string& ext) {
  std::vector<boost::filesystem::path> filesList;

  if (boost::filesystem::exists(root) && boost::filesystem::is_directory(root)) {
    boost::filesystem::recursive_directory_iterator it(root);
    boost::filesystem::recursive_directory_iterator endit;
    while (it != endit) {
      if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ext) {
        filesList.push_back(it->path().filename());
      }
      ++it;
    }
  }
  return filesList;
}

// Return the full path of the file where the batch with uuid number will be stored.
std::string Generation::MakeBatchPath(std::string disk_path, boost::uuids::uuid uuid) {
  boost::filesystem::path dir(disk_path);
  if (!boost::filesystem::is_directory(dir)) {
    bool is_created = boost::filesystem::create_directory(dir);
  }
  std::string batch_extension = ".batch";
  boost::filesystem::path file(boost::lexical_cast<std::string>(uuid) + batch_extension);
  boost::filesystem::path batch_path = dir / file;
  std::string batch_file = batch_path.string();
  return batch_file;
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

