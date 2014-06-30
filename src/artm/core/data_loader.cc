// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/data_loader.h"

#include <string>
#include <vector>
#include <fstream>  // NOLINT

#include "boost/lexical_cast.hpp"
#include "boost/uuid/uuid_io.hpp"

#include "glog/logging.h"

#include "rpcz/application.hpp"

#include "artm/core/exceptions.h"
#include "artm/core/instance.h"
#include "artm/core/batch_manager.h"
#include "artm/core/instance_schema.h"
#include "artm/core/internals.rpcz.h"
#include "artm/core/protobuf_helpers.h"
#include "artm/core/helpers.h"
#include "artm/core/zmq_context.h"
#include "artm/core/generation.h"
#include "artm/core/merger.h"

namespace artm {
namespace core {

DataLoader::DataLoader(Instance* instance)
    : instance_(instance) { }

Instance* DataLoader::instance() {
  return instance_;
}

void DataLoader::PopulateDataStreams(const Batch& batch, ProcessorInput* pi) {
  // loop through all streams
  MasterComponentConfig config = instance()->schema()->config();
  for (int stream_index = 0; stream_index < config.stream_size(); ++stream_index) {
    const Stream& stream = config.stream(stream_index);
    pi->add_stream_name(stream.name());

    Mask* mask = pi->add_stream_mask();
    for (int item_index = 0; item_index < batch.item_size(); ++item_index) {
      // verify if item is part of the stream
      bool value = false;
      switch (stream.type()) {
        case Stream_Type_Global: {
          value = true;
          break;  // Stream_Type_Global
        }

        case Stream_Type_ItemIdModulus: {
          int id_mod = batch.item(item_index).id() % stream.modulus();
          value = repeated_field_contains(stream.residuals(), id_mod);
          break;  // Stream_Type_ItemIdModulus
        }

        default:
          BOOST_THROW_EXCEPTION(NotImplementedException("Stream_Type_ItemHashModulus"));
      }

      mask->add_value(value);
    }
  }
}

LocalDataLoader::LocalDataLoader(Instance* instance)
    : DataLoader(instance),
      generation_(std::make_shared<Generation>(instance->schema()->config().disk_path())),
      cache_(),
      is_stopping(false),
      thread_() {
  // Keep this at the last action in constructor.
  // http://stackoverflow.com/questions/15751618/initialize-boost-thread-in-object-constructor
  boost::thread t(&LocalDataLoader::ThreadFunction, this);
  thread_.swap(t);
}

LocalDataLoader::~LocalDataLoader() {
  is_stopping = true;
  if (thread_.joinable()) {
    thread_.join();
  }
}

void LocalDataLoader::AddBatch(const Batch& batch) {
  MasterComponentConfig config = instance()->schema()->config();
  std::shared_ptr<Generation> next_gen = generation_.get_copy();
  if (config.compact_batches()) {
    Batch compacted_batch;
    CompactBatch(batch, &compacted_batch);
    next_gen->AddBatch(std::make_shared<Batch>(compacted_batch), config.disk_path());
  } else {
    next_gen->AddBatch(std::make_shared<Batch>(batch), config.disk_path());
  }

  generation_.set(next_gen);
}

void LocalDataLoader::CompactBatch(const Batch& batch, Batch* compacted_batch) {
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

int LocalDataLoader::GetTotalItemsCount() const {
  auto ptr = generation_.get();
  return ptr->GetTotalItemsCount();
}

void LocalDataLoader::InvokeIteration(int iterations_count) {
  if (iterations_count <= 0) {
    LOG(WARNING) << "DataLoader::InvokeIteration() was called with argument '"
                 << iterations_count << "'. Call is ignored.";
    return;
  }

  // Reset scores
  instance()->merger()->ForceResetScores(ModelName());

  auto latest_generation = generation_.get();
  if (latest_generation->empty()) {
    LOG(WARNING) << "DataLoader::InvokeIteration() - current generation is empty, "
                 << "please populate DataLoader data with some data";
    return;
  }

  for (int iter = 0; iter < iterations_count; ++iter) {
    latest_generation->InvokeOnEachPartition(
      [&](boost::uuids::uuid uuid, std::shared_ptr<const Batch> batch) {
        instance_->batch_manager()->Add(uuid);
      });
  }
}

void LocalDataLoader::WaitIdle() {
  for (;;) {
    if (instance_->batch_manager()->IsEverythingProcessed())
      break;

    boost::this_thread::sleep(boost::posix_time::milliseconds(1));
  }

  instance()->merger()->ForcePushTopicModelIncrement();
  instance()->merger()->ForcePullTopicModel();
}

void LocalDataLoader::DisposeModel(ModelName model_name) {
  auto keys = cache_.keys();
  for (auto &key : keys) {
    auto cache_entry = cache_.get(key);
    if (cache_entry == nullptr) {
      continue;
    }

    if (cache_entry->model_name() == model_name) {
      cache_.erase(key);
    }
  }
}

bool LocalDataLoader::RequestThetaMatrix(ModelName model_name, ::artm::ThetaMatrix* theta_matrix) {
  std::shared_ptr<Generation> generation = generation_.get();
  std::vector<boost::uuids::uuid> batch_uuids = generation->batch_uuids();

  theta_matrix->set_model_name(model_name);
  for (auto &batch_uuid : batch_uuids) {
    auto cache = cache_.get(CacheKey(batch_uuid, model_name));
    if (cache == nullptr) {
      LOG(INFO) << "Unable to find cache entry for model: " << model_name << ", batch: " << batch_uuid;
      continue;
    }

    for (int item_index = 0; item_index < cache->item_id_size(); ++item_index) {
      theta_matrix->add_item_id(cache->item_id(item_index));
      theta_matrix->add_item_weights()->CopyFrom(cache->theta(item_index));
    }
  }

  return true;
}

void LocalDataLoader::Callback(std::shared_ptr<const ProcessorOutput> cache) {
  MasterComponentConfig config = instance()->schema()->config();
  boost::uuids::uuid uuid(boost::uuids::string_generator()(cache->batch_uuid().c_str()));
  instance_->batch_manager()->Done(uuid);
  if (config.cache_processor_output()) {
    for (int model_index = 0; model_index < cache->model_increment_size(); model_index++) {
      const ModelIncrement& model_increment = cache->model_increment(model_index);
      ModelName model_name = model_increment.model_name();
      CacheKey cache_key(uuid, model_name);
      std::shared_ptr<DataLoaderCacheEntry> cache_entry(new DataLoaderCacheEntry());
      cache_entry->set_batch_uuid(cache->batch_uuid());
      cache_entry->set_model_name(model_name);
      for (int item_index = 0; item_index < model_increment.item_id_size(); ++item_index) {
        cache_entry->add_item_id(model_increment.item_id(item_index));
        cache_entry->add_theta()->CopyFrom(model_increment.theta(item_index));
      }

      cache_.set(cache_key, cache_entry);
    }
  }
}

void LocalDataLoader::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "DataLoader thread");
    LOG(INFO) << "DataLoader thread started";
    for (;;) {
      if (is_stopping) {
        break;
      }

      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      auto schema = instance()->schema();
      auto config = schema->config();

      if (instance()->processor_queue()->size() >= config.processor_queue_max_size())
        continue;

      boost::uuids::uuid next_batch_uuid = instance_->batch_manager()->Next();
      if (next_batch_uuid.is_nil())
        continue;

      auto latest_generation = generation_.get();
      std::shared_ptr<const Batch> batch = latest_generation->batch(next_batch_uuid,
                                                                    config.disk_path());
      if (batch == nullptr) {
        instance_->batch_manager()->Done(next_batch_uuid);
        continue;
      }

      auto pi = std::make_shared<ProcessorInput>();
      pi->mutable_batch()->CopyFrom(*batch);
      pi->set_batch_uuid(boost::lexical_cast<std::string>(next_batch_uuid));

      auto keys = cache_.keys();
      for (auto &key : keys) {
        auto cache_entry = cache_.get(key);
        if (cache_entry == nullptr) {
          continue;
        }

        if (cache_entry->batch_uuid() == pi->batch_uuid()) {
          pi->add_cached_theta()->CopyFrom(*cache_entry);
        }
      }

      DataLoader::PopulateDataStreams(*batch, pi.get());
      instance()->processor_queue()->push(pi);
    }
  }
  catch(boost::thread_interrupted&) {
    LOG(WARNING) << "thread_interrupted exception in LocalDataLoader::ThreadFunction() function";
    return;
  }
  catch(...) {
    LOG(FATAL) << "Fatal exception in LocalDataLoader::ThreadFunction() function";
    throw;
  }
}

RemoteDataLoader::RemoteDataLoader(Instance* instance)
    : DataLoader(instance),
      is_stopping(false),
      thread_() {
  // Keep this at the last action in constructor.
  // http://stackoverflow.com/questions/15751618/initialize-boost-thread-in-object-constructor
  boost::thread t(&RemoteDataLoader::ThreadFunction, this);
  thread_.swap(t);
}

RemoteDataLoader::~RemoteDataLoader() {
  is_stopping = true;
  if (thread_.joinable()) {
    thread_.join();
  }
}

void RemoteDataLoader::Callback(std::shared_ptr<const ProcessorOutput> cache) {
  // ToDo(alfrey): implement Theta-caching in network modus operandi

  BatchIds processed_batches;
  processed_batches.add_batch_id(cache->batch_uuid());
  Void response;
  try {
    instance()->master_component_service_proxy()->ReportBatches(processed_batches, &response);
  } catch(...) {
    LOG(ERROR) << "Unable to report processed batches to master.";
  }
}

void RemoteDataLoader::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "DataLoader thread");
    LOG(INFO) << "DataLoader thread started";
    for (;;) {
      if (is_stopping) {
        break;
      }

      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      MasterComponentConfig config = instance()->schema()->config();
      int processor_queue_size = instance()->processor_queue()->size();
      int max_queue_size = config.processor_queue_max_size();
      if (processor_queue_size >= max_queue_size)
        continue;

      Int request;  // desired number of batches
      BatchIds response;
      request.set_value(max_queue_size - processor_queue_size);
      try {
        instance()->master_component_service_proxy()->RequestBatches(request, &response);
      } catch(const std::runtime_error& exception) {
        LOG(ERROR) << exception.what();
      } catch(...) {
        LOG(ERROR) << "Unable to request batches from master.";
        return;
      }

      BatchIds failed_batches;
      for (int batch_index = 0; batch_index < response.batch_id_size(); ++batch_index) {
        std::string batch_id = response.batch_id(batch_index);
        boost::uuids::uuid next_batch_uuid(boost::uuids::string_generator()(batch_id.c_str()));
        std::shared_ptr<const Batch> batch =
          Generation::LoadBatch(next_batch_uuid, config.disk_path());

        if (batch == nullptr) {
          LOG(ERROR) << "Unable to load batch '" << batch_id << "' from " << config.disk_path();
          failed_batches.add_batch_id(batch_id);
          continue;
        }

        auto pi = std::make_shared<ProcessorInput>();
        pi->mutable_batch()->CopyFrom(*batch);
        pi->set_batch_uuid(boost::lexical_cast<std::string>(next_batch_uuid));

        // ToDo(alfrey): implement Theta-caching in network modus operandi
        DataLoader::PopulateDataStreams(*batch, pi.get());
        instance()->processor_queue()->push(pi);
      }

      if (failed_batches.batch_id_size() > 0) {
        Void response;
        try {
          instance()->master_component_service_proxy()->ReportBatches(failed_batches, &response);
        } catch(...) {
          LOG(ERROR) << "Unable to report failed batches to master.";
        }
      }
    }
  }
  catch(boost::thread_interrupted&) {
    LOG(WARNING) << "thread_interrupted exception in RemoteDataLoader::ThreadFunction() function";
    return;
  }
  catch(...) {
    LOG(FATAL) << "Fatal exception in RemoteDataLoader::ThreadFunction() function";
    throw;
  }
}

}  // namespace core
}  // namespace artm
