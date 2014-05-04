// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/merger.h"

#include <algorithm>

#include "boost/lexical_cast.hpp"

#include "glog/logging.h"

#include "artm/common.h"
#include "artm/call_on_destruction.h"
#include "artm/data_loader.h"
#include "artm/exceptions.h"
#include "artm/helpers.h"

#include "rpcz/rpc.hpp"

using ::artm::memcached::MemcachedService_Stub;

namespace artm {
namespace core {

Merger::Merger(boost::mutex* merger_queue_lock,
               std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue,
               ThreadSafeHolder<InstanceSchema>* schema,
               ThreadSafeHolder<artm::memcached::MemcachedService_Stub>* memcached_service)
    : lock_(),
      topic_model_(lock_),
      schema_(schema),
      memcached_service_(memcached_service),
      merger_queue_lock_(merger_queue_lock),
      merger_queue_(merger_queue),
      thread_() {
  // Keep this at the last action in constructor.
  // http://stackoverflow.com/questions/15751618/initialize-boost-thread-in-object-constructor
  boost::thread t(&Merger::ThreadFunction, this);
  thread_.swap(t);
}

Merger::~Merger() {
  if (thread_.joinable()) {
    thread_.interrupt();
    thread_.join();
  }
}

void Merger::DisposeModel(ModelId model_id) {
  topic_model_.erase(model_id);
}

void Merger::UpdateModel(const ModelConfig& model) {
  if (!topic_model_.has_key(model.model_id())) {
    // Handle more type of reconfigs - for example, changing the number of topics;
    auto ttm = std::make_shared<TopicModel>(model.model_id(), model.topics_count(),
                                            model.score_size());
    topic_model_.set(model.model_id(), ttm);
  }

  auto ttm = topic_model_.get(model.model_id());
  if (ttm->topic_size() != model.topics_count()) {
    std::string message("Unable to change the number of topics in topic model");
    BOOST_THROW_EXCEPTION(UnsupportedReconfiguration(message));
  }
}

std::shared_ptr<const ::artm::core::TopicModel>
Merger::GetLatestTopicModel(ModelId model_id) const {
  return topic_model_.get(model_id);
}

void Merger::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "Merger thread");
    LOG(INFO) << "Merger thread started";
    for (;;) {
      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      // Merge everything from the queue and update topic model.
      for (;;) {
        std::shared_ptr<const ProcessorOutput> processor_output;
        {
          boost::lock_guard<boost::mutex> guard(*merger_queue_lock_);
          if (merger_queue_->empty()) {
            break;
          }

          processor_output = merger_queue_->front();
          merger_queue_->pop();
        }

        helpers::call_on_destruction c([&]() {
          // Callback to DataLoader
          auto data_loader = DataLoaderManager::singleton().Get(
            processor_output->data_loader_id());

          if (data_loader != nullptr) {
            data_loader->Callback(processor_output);
          }
        });

        for (int modex_index = 0;
             modex_index < processor_output->model_increment_size();
             modex_index++) {
          auto model_increment = processor_output->model_increment(modex_index);
          ModelId model_id = model_increment.model_id();
          auto cur_ttm = topic_model_.get(model_id);
          if (cur_ttm.get() == nullptr) {
            // a model had been disposed during ongoing processing;
            continue;
          }

          auto new_ttm = std::make_shared<TopicModel>(*cur_ttm);
          new_ttm->ApplyDiff(model_increment);

          std::shared_ptr<MemcachedService_Stub> memcached_service = memcached_service_->get();
          if (memcached_service != nullptr) {
            SyncWithMemcached(*cur_ttm, new_ttm.get(), memcached_service.get());
          }

          topic_model_.set(model_id, new_ttm);
        }
      }
    }
  }
  catch(boost::thread_interrupted&) {
    LOG(WARNING) << "thread_interrupted exception in Merger::ThreadFunction() function";
    return;
  } catch(...) {
    LOG(FATAL) << "Fatal exception in Merger::ThreadFunction() function";
    throw;
  }
}

void Merger::SyncWithMemcached(const ::artm::core::TopicModel& old_ttm,
                               ::artm::core::TopicModel* new_ttm,
                               artm::memcached::MemcachedService_Stub* memcached_proxy) {
  // This method (SyncWithMemcached) calculates a diff between new_ttm and old_ttm,
  // and then sends it to MemcachedService as ModelIncrement.

  ModelIncrement model_increment;
  new_ttm->CalculateDiff(old_ttm, &model_increment);

  try {
    ::artm::TopicModel new_global_ttm;
    memcached_proxy->UpdateModel(model_increment, &new_global_ttm);
    new_ttm->CopyFromExternalTopicModel(new_global_ttm);
  } catch(const rpcz::rpc_error&) {
    LOG(ERROR) << "Merger failed to send updates to memcached service.";
    throw;
  }
}

}  // namespace core
}  // namespace artm

