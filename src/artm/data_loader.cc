// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/data_loader.h"

#include <string>
#include <vector>
#include <fstream>  // NOLINT

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid_io.hpp"

#include "glog/logging.h"

#include "artm/exceptions.h"
#include "artm/protobuf_helpers.h"
#include "artm/helpers.h"

namespace artm {
namespace core {

DataLoader::DataLoader(int id, const DataLoaderConfig& config)
    : data_loader_id_(id),
      lock_(),
      config_(lock_, std::make_shared<DataLoaderConfig>(config)),
      generation_(lock_, std::make_shared<Generation>(config.disk_path())),
      cache_lock_(),
      cache_(cache_lock_),
      batch_manager_lock_(),
      batch_manager_(&batch_manager_lock_),
      thread_() {
  // Keep this at the last action in constructor.
  // http://stackoverflow.com/questions/15751618/initialize-boost-thread-in-object-constructor
  boost::thread t(&DataLoader::ThreadFunction, this);
  thread_.swap(t);
}

DataLoader::~DataLoader() {
  if (thread_.joinable()) {
    thread_.interrupt();
    thread_.join();
  }
}

void DataLoader::Interrupt() {
  thread_.interrupt();
}

void DataLoader::Join() {
  thread_.join();
}

void DataLoader::Reconfigure(const DataLoaderConfig& config) {
  config_.set(std::make_shared<DataLoaderConfig>(config));
}

void DataLoader::AddBatch(const Batch& batch) {
  std::shared_ptr<Generation> next_gen = generation_.get_copy();
  if (config_.get()->compact_batches()) {
    Batch compacted_batch;
    CompactBatch(batch, &compacted_batch);
    next_gen->AddBatch(std::make_shared<Batch>(compacted_batch), config_.get()->disk_path());
  } else {
    next_gen->AddBatch(std::make_shared<Batch>(batch), config_.get()->disk_path());
  }

  generation_.set(next_gen);
}

void DataLoader::CompactBatch(const Batch& batch, Batch* compacted_batch) {
  std::vector<int> orig_to_compacted_id_map(batch.token_size(), -1);
  int compacted_dictionary_size = 0;

  for (int item_index = 0; item_index < batch.item_size(); ++item_index) {
    auto item = batch.item(item_index);
    auto compacted_item = compacted_batch->add_item();
    compacted_item->CopyFrom(item);

    for (int field_index = 0; field_index < item.field_size(); ++field_index) {
      auto field = item.field(field_index);
      auto compacted_field = compacted_item->mutable_field(field_index);

      for (int token_index = 0; token_index < field.token_id_size(); ++token_index) {
        int token_id = field.token_id(token_index);
        if (token_id < 0 || token_id >= batch.token_size())
          BOOST_THROW_EXCEPTION(ArgumentOutOfRangeException("field.token_id"));

        if (orig_to_compacted_id_map[token_id] == -1) {
          orig_to_compacted_id_map[token_id] = compacted_dictionary_size++;
          compacted_batch->add_token(batch.token(token_id));
        }

        compacted_field->set_token_id(token_index, orig_to_compacted_id_map[token_id]);
      }
    }
  }
}

int DataLoader::GetTotalItemsCount() const {
  auto ptr = generation_.get();
  return ptr->GetTotalItemsCount();
}

int DataLoader::id() const {
  return data_loader_id_;
}

void DataLoader::InvokeIteration(int iterations_count) {
  if (iterations_count <= 0) {
    LOG(WARNING) << "DataLoader::InvokeIteration() was called with argument '"
                 << iterations_count << "'. Call is ignored.";
    return;
  }

  // Reset scores
  auto instance = InstanceManager::singleton().Get(config_.get()->instance_id());
  if (instance != nullptr) {
    instance->ForceResetScores(ModelId());
  }

  auto latest_generation = generation_.get();
  if (latest_generation->empty()) {
    LOG(WARNING) << "DataLoader::InvokeIteration() - current generation is empty, "
                 << "please populate DataLoader data with some data";
    return;
  }

  for (int iter = 0; iter < iterations_count; ++iter) {
    latest_generation->InvokeOnEachPartition(
      [&](boost::uuids::uuid uuid, std::shared_ptr<const Batch> batch) {
        batch_manager_.Add(uuid);
      });
  }
}

void DataLoader::WaitIdle() {
  for (;;) {
    if (batch_manager_.IsEverythingProcessed())
      break;

    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }

  auto instance = InstanceManager::singleton().Get(config_.get()->instance_id());
  if (instance == nullptr)
    return;

  instance->ForceSyncWithMemcached(ModelId());
}

void DataLoader::Callback(std::shared_ptr<const ProcessorOutput> cache) {
  boost::uuids::uuid uuid(boost::uuids::string_generator()(cache->batch_uuid().c_str()));
  batch_manager_.Done(uuid);
  if (config_.get()->cache_processor_output()) {
    cache_.set(uuid, cache);
  }
}

void DataLoader::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "DataLoader thread");
    LOG(INFO) << "DataLoader thread started";
    for (;;) {
      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      auto config = config_.get();

      auto instance = InstanceManager::singleton().Get(config->instance_id());
      if (instance == nullptr)
        continue;

      if (instance->processor_queue_size() >= config->queue_size())
        continue;

      boost::uuids::uuid next_batch_uuid = batch_manager_.Next();
      if (next_batch_uuid.is_nil())
        continue;

      auto latest_generation = generation_.get();
      std::shared_ptr<const Batch> batch = latest_generation->batch(next_batch_uuid,
                                                                    config_.get()->disk_path());
      if (batch == nullptr) {
        batch_manager_.Done(next_batch_uuid);
        continue;
      }

      auto pi = std::make_shared<ProcessorInput>();
      pi->mutable_batch()->CopyFrom(*batch);
      pi->set_batch_uuid(boost::lexical_cast<std::string>(next_batch_uuid));
      pi->set_data_loader_id(id());

      auto cache_entry = cache_.get(next_batch_uuid);
      if (cache_entry != nullptr) {
        pi->mutable_previous_processor_output()->CopyFrom(*cache_entry);
      }

      // loop through all streams
      for (int stream_index = 0; stream_index < config->stream_size(); ++stream_index) {
        const Stream& stream = config->stream(stream_index);
        pi->add_stream_name(stream.name());

        Mask* mask = pi->add_stream_mask();
        for (int item_index = 0; item_index < batch->item_size(); ++item_index) {
          // verify if item is part of the stream
          bool value = false;
          switch (stream.type()) {
            case Stream_Type_Global: {
              value = true;
              break;  // Stream_Type_Global
            }

            case Stream_Type_ItemIdModulus: {
              int id_mod = batch->item(item_index).id() % stream.modulus();
              value = repeated_field_contains(stream.residuals(), id_mod);
              break;  // Stream_Type_ItemIdModulus
            }

            default:
              BOOST_THROW_EXCEPTION(NotImplementedException("Stream_Type_ItemHashModulus"));
          }

          mask->add_value(value);
        }
      }

      instance->AddBatchIntoProcessorQueue(pi);
    }
  }
  catch(boost::thread_interrupted&) {
    LOG(WARNING) << "thread_interrupted exception in DataLoader::ThreadFunction() function";
    return;
  }
  catch(...) {
    LOG(FATAL) << "Fatal exception in DataLoader::ThreadFunction() function";
    throw;
  }
}

}  // namespace core
}  // namespace artm
