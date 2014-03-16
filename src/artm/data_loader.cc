// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/data_loader.h"

#include <string>

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid_io.hpp"

#include "artm/exceptions.h"
#include "artm/protobuf_helpers.h"
#include "artm/helpers.h"

namespace artm {
namespace core {

DataLoader::DataLoader(int id, const DataLoaderConfig& config)
    : data_loader_id_(id),
      lock_(),
      config_(lock_, std::make_shared<DataLoaderConfig>(config)),
      generation_(lock_, std::make_shared<Generation>()),
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
  next_gen->AddBatch(std::make_shared<Batch>(batch));
  generation_.set(next_gen);
}

int DataLoader::GetTotalItemsCount() const {
  auto ptr = generation_.get();
  return ptr->GetTotalItemsCount();
}

int DataLoader::id() const {
  return data_loader_id_;
}

DataLoader::BatchManager::BatchManager(boost::mutex* lock)
    : lock_(lock), tasks_(), in_progress_() {}

void DataLoader::BatchManager::Add(const boost::uuids::uuid& id) {
  boost::lock_guard<boost::mutex> guard(*lock_);
  tasks_.push_back(id);
}

boost::uuids::uuid DataLoader::BatchManager::Next() {
  boost::lock_guard<boost::mutex> guard(*lock_);
  for (auto iter = tasks_.begin(); iter != tasks_.end(); ++iter) {
    if (in_progress_.find(*iter) == in_progress_.end()) {
      boost::uuids::uuid retval = *iter;
      tasks_.erase(iter);
      in_progress_.insert(retval);
      return retval;
    }
  }

  return boost::uuids::uuid();
}

void DataLoader::BatchManager::Done(const boost::uuids::uuid& id) {
  boost::lock_guard<boost::mutex> guard(*lock_);
  in_progress_.erase(id);
}

bool DataLoader::BatchManager::IsEverythingProcessed() const {
  boost::lock_guard<boost::mutex> guard(*lock_);
  return (tasks_.empty() && in_progress_.empty());
}

void DataLoader::InvokeIteration(int iterations_count) {
  if (iterations_count <= 0) {
    // ToDo(alfrey) Log a warning
    return;
  }

  auto latest_generation = generation_.get();
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
      return;

    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }
}

void DataLoader::Callback(std::shared_ptr<const ProcessorOutput> cache) {
  boost::uuids::uuid uuid(boost::uuids::string_generator()(cache->batch_uuid().c_str()));
  batch_manager_.Done(uuid);
  cache_.set(uuid, cache);
}

void DataLoader::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "DataLoader thread");
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
      std::shared_ptr<const Batch> batch = latest_generation->batch(next_batch_uuid);
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

            case Stream_Type_ItemHashModulus:
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
    return;
  }
}

}  // namespace core
}  // namespace artm
