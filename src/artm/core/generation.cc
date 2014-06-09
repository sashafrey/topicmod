// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/generation.h"

#include <fstream>  // NOLINT

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/filesystem.hpp"

#include "glog/logging.h"

#include "artm/core/common.h"

namespace artm {
namespace core {

Generation::Generation(const std::string& disk_path) : id_(0), generation_() {
  if (!disk_path.empty()) {
    std::vector<boost::uuids::uuid> batch_uuids = ListAllBatches(disk_path);
    for (size_t i = 0; i < batch_uuids.size(); ++i) {
      generation_.insert(std::make_pair(batch_uuids[i], nullptr));
    }
  }
}

int Generation::id() const {
  return id_;
}

bool Generation::empty() const {
  return generation_.empty();
}

std::shared_ptr<const Batch> Generation::LoadBatch(const boost::uuids::uuid& uuid,
                                                   const std::string& disk_path) {
  std::shared_ptr<const Batch> batch_ptr = nullptr;
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

  return batch_ptr;
}

std::shared_ptr<const Batch> Generation::batch(const boost::uuids::uuid& uuid,
                                               const std::string& disk_path) {
  std::shared_ptr<const Batch> batch_ptr = nullptr;
  auto retval = generation_.find(uuid);
  if (retval != generation_.end()) {
    if (disk_path.empty()) {
      batch_ptr = retval->second;
    } else {
      return LoadBatch(uuid, disk_path);
    }
  }
  return batch_ptr;
}

std::vector<boost::uuids::uuid> Generation::batch_uuids() const {
  std::vector<boost::uuids::uuid> retval;
  for (auto iter = generation_.begin(); iter != generation_.end(); ++iter) {
    retval.push_back(iter->first);
  }

  return std::move(retval);
}

boost::uuids::uuid Generation::SaveBatch(const Batch& batch,
                                         const std::string& disk_path) {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  std::string batch_file = MakeBatchPath(disk_path, uuid);
  std::ofstream fout(batch_file.c_str(), std::ofstream::binary);
  if (fout.is_open()) {
    bool is_serialized = batch.SerializeToOstream(&fout);
    if (!is_serialized) {
      LOG(ERROR) << "Batch has not been serialized on disk.";
    }

    fout.close();
  }

  return uuid;
}

void Generation::AddBatch(const std::shared_ptr<const Batch>& batch,
                          const std::string& disk_path) {
  if (disk_path.empty()) {
    generation_.insert(std::make_pair(boost::uuids::random_generator()(), batch));
  } else {
    boost::uuids::uuid uuid = SaveBatch(*batch, disk_path);
    generation_.insert(std::make_pair(uuid, nullptr));
  }
}

// Return the filenames of all files that have the specified extension
// in the specified directory.
std::vector<boost::uuids::uuid> Generation::ListAllBatches(const boost::filesystem::path& root) {
  std::vector<boost::uuids::uuid> uuids;

  if (boost::filesystem::exists(root) && boost::filesystem::is_directory(root)) {
    boost::filesystem::recursive_directory_iterator it(root);
    boost::filesystem::recursive_directory_iterator endit;
    while (it != endit) {
      if (boost::filesystem::is_regular_file(*it) && it->path().extension() == kBatchExtension) {
        std::string filename = it->path().filename().stem().string();
        boost::uuids::uuid uuid = boost::uuids::string_generator()(filename);
        if (uuid.is_nil()) {
          LOG(WARNING) << "Unable to convert filename " << filename << " to uuid.";
          continue;
        }

        uuids.push_back(uuid);
      }
      ++it;
    }
  }
  return uuids;
}

// Return the full path of the file where the batch with uuid number will be stored.
std::string Generation::MakeBatchPath(std::string disk_path, boost::uuids::uuid uuid) {
  boost::filesystem::path dir(disk_path);
  if (!boost::filesystem::is_directory(dir)) {
    bool is_created = boost::filesystem::create_directory(dir);
    if (!is_created) {
      LOG(ERROR) << "Unable to create folder '" << dir << "'";
    }
  }

  boost::filesystem::path file(boost::lexical_cast<std::string>(uuid) + kBatchExtension);
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

