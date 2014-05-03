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
          new_ttm->IncreaseItemsProcessed(model_increment.items_processed());
          for (int score_index = 0; score_index < model_increment.score_size(); ++score_index) {
            new_ttm->IncreaseScores(score_index, model_increment.score(score_index),
                                    model_increment.score_norm(score_index));
          }

          // Add new tokens discovered by processor
          for (int token_index = 0;
               token_index < model_increment.discovered_token_size();
               ++token_index) {
            std::string new_token = model_increment.discovered_token(token_index);
            if (!new_ttm->has_token(new_token)) {
              new_ttm->AddToken(new_token);
            }
          }

          int topics_count = new_ttm->topic_size();

          for (int token_index = 0;
               token_index < model_increment.token_increment_size();
               ++token_index) {
            const FloatArray& counters = model_increment.token_increment(token_index);
            const std::string& token = model_increment.token(token_index);
            for (int topic_index = 0; topic_index < topics_count; ++topic_index) {
              new_ttm->IncreaseTokenWeight(token, topic_index, counters.value(topic_index));
            }
          }

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

// ToDo(alfrey): do we need some special sync for newly added tokens?
void Merger::SyncWithMemcached(const TopicModel& old_ttm, TopicModel* new_ttm,
                               artm::memcached::MemcachedService_Stub* memcached_proxy) {
  const std::string kPrefixToken = "tk_";
  const std::string kPrefixScore = "sc_";  // todo(alfrey) scores should also have GUIDs.
  const std::string kPrefixItemsProcessed = "ip_";

  assert(old_ttm.topic_size() == new_ttm->topic_size());
  int topic_size = new_ttm->topic_size();

  int timeout = -1;

  // ToDo(alfrey): model should be identified by a guid, consistent across all instances.
  std::string key_group = "<model_guid>";

  // 1. Synchronize tokens.
  for (int token_index = 0; token_index < new_ttm->token_size(); ++token_index) {
    const std::string& token = new_ttm->token(token_index);
    auto new_iter = new_ttm->GetTopicWeightIterator(token_index);

    artm::memcached::UpdateKeyArgs update_key_args;
    update_key_args.set_key_group(key_group);
    update_key_args.set_key(kPrefixToken + token);

    if (old_ttm.has_token(token)) {
      auto old_iter = old_ttm.GetTopicWeightIterator(token);
      while ((old_iter.NextTopic() < topic_size) && (new_iter.NextTopic() < topic_size)) {
        update_key_args.add_value(new_iter.NotNormalizedWeight() - old_iter.NotNormalizedWeight());
      }
    } else {
      while (new_iter.NextTopic() < topic_size) {
        update_key_args.add_value(new_iter.NotNormalizedWeight());
      }
    }

    artm::memcached::UpdateKeyResult update_key_result;
    try {
      memcached_proxy->UpdateKey(update_key_args, &update_key_result, timeout);
    } catch(const rpcz::rpc_error&) {
      LOG(ERROR) << "Merger failed to send updates to memcached service.";
      continue;
    }

    if (update_key_result.error_code() != artm::memcached::kSuccess) {
      LOG(ERROR) << "Merger failed to send updates to memcached service.";
      continue;
    }

    if (update_key_result.value_size() != topic_size) {
      LOG(ERROR) << "Merger failed to send updates to memcached service.";
      continue;
    }

    for (int topic_index = 0; topic_index < topic_size; ++topic_index) {
      new_ttm->SetTokenWeight(token_index, topic_index, update_key_result.value(topic_index));
    }
  }

  // 2. Synchronize scores.
  for (int score_index = 0; score_index < new_ttm->score_size(); ++score_index) {
    artm::memcached::UpdateKeyArgs update_key_args;
    update_key_args.set_key_group(key_group);

    // ToDo(alfrey): score_guid
    update_key_args.set_key(kPrefixScore + boost::lexical_cast<std::string>(score_index));

    update_key_args.add_value(static_cast<float>(
        new_ttm->score_not_normalized(score_index) - old_ttm.score_not_normalized(score_index)));
    update_key_args.add_value(static_cast<float>(
      new_ttm->score_normalizer(score_index) - old_ttm.score_normalizer(score_index)));

    artm::memcached::UpdateKeyResult update_key_result;
    try {
      memcached_proxy->UpdateKey(update_key_args, &update_key_result, timeout);
    } catch(const rpcz::rpc_error&) {
      LOG(ERROR) << "Merger failed to send updates to memcached service.";
      continue;
    }

    if (update_key_result.error_code() != artm::memcached::kSuccess) {
      LOG(ERROR) << "Merger failed to send updates to memcached service.";
      continue;
    }

    new_ttm->SetScores(score_index, update_key_result.value(0), update_key_result.value(1));
  }

  // 3. Synchronize items processed.
  {
    artm::memcached::UpdateKeyArgs update_key_args;
    update_key_args.set_key_group(key_group);
    update_key_args.set_key(kPrefixItemsProcessed);

    update_key_args.add_value(static_cast<float>(
      new_ttm->items_processed() - old_ttm.items_processed()));

    artm::memcached::UpdateKeyResult update_key_result;
    try {
      memcached_proxy->UpdateKey(update_key_args, &update_key_result, timeout);
    } catch(const rpcz::rpc_error&) {
      LOG(ERROR) << "Merger failed to send updates to memcached service.";
      return;
    }

    if (update_key_result.error_code() != artm::memcached::kSuccess) {
      LOG(ERROR) << "Merger failed to send updates to memcached service.";
      return;
    }

    new_ttm->SetItemsProcessed(static_cast<int>(update_key_result.value(0)));
  }
}

}  // namespace core
}  // namespace artm

