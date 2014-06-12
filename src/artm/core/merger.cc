// Copyright 2014, Additive Regularization of Topic Models.

#include "artm/core/merger.h"

#include <algorithm>

#include "boost/lexical_cast.hpp"

#include "glog/logging.h"

#include "rpcz/rpc.hpp"

#include "artm/regularizer_interface.h"
#include "artm/core/common.h"
#include "artm/core/call_on_destruction.h"
#include "artm/core/data_loader.h"
#include "artm/core/exceptions.h"
#include "artm/core/helpers.h"
#include "artm/core/topic_model.h"
#include "artm/core/instance_schema.h"

using ::artm::core::MasterComponentService_Stub;

namespace artm {
namespace core {

Merger::Merger(boost::mutex* merger_queue_lock,
               std::queue<std::shared_ptr<const ProcessorOutput> >* merger_queue,
               ThreadSafeHolder<InstanceSchema>* schema,
               ThreadSafeHolder<artm::core::MasterComponentService_Stub>* master_component_service)
    : lock_(),
      topic_model_(lock_),
      new_topic_model_(),
      schema_(schema),
      master_component_service_(master_component_service),
      merger_queue_lock_(merger_queue_lock),
      merger_queue_(merger_queue),
      is_stopping(false),
      thread_() {
  // Keep this at the last action in constructor.
  // http://stackoverflow.com/questions/15751618/initialize-boost-thread-in-object-constructor
  boost::thread t(&Merger::ThreadFunction, this);
  thread_.swap(t);
}

Merger::~Merger() {
  is_stopping = true;
  if (thread_.joinable()) {
    thread_.join();
  }
}

void Merger::DisposeModel(ModelName model_name) {
  topic_model_.erase(model_name);
  internal_task_queue_.push(MergerTask(kDisposeModel, model_name));
}

void Merger::UpdateModel(const ModelConfig& model) {
  if (!topic_model_.has_key(model.name())) {
    // Handle more type of reconfigs - for example, changing the number of topics;
    auto ttm = std::make_shared<TopicModel>(model.name(), model.topics_count(),
                                            model.score_size());
    topic_model_.set(model.name(), ttm);
  }

  auto ttm = topic_model_.get(model.name());
  if (ttm->topic_size() != model.topics_count()) {
    std::string message("Unable to change the number of topics in topic model");
    BOOST_THROW_EXCEPTION(UnsupportedReconfiguration(message));
  }
}

void Merger::ForceResetScores(ModelName model_name) {
  rpcz::sync_event sync_event;
  internal_task_queue_.push(MergerTask(kForceResetScores, model_name, &sync_event));
  sync_event.wait();
}

void Merger::ForceSyncWithMemcached(ModelName model_name) {
  rpcz::sync_event sync_event;
  internal_task_queue_.push(MergerTask(kForceSyncWithMemcached, model_name, &sync_event));
  sync_event.wait();
}

std::shared_ptr<const ::artm::core::TopicModel>
Merger::GetLatestTopicModel(ModelName model_name) const {
  return topic_model_.get(model_name);
}

void Merger::InvokePhiRegularizers() {
  auto schema = schema_->get();
  std::vector<ModelName> model_names = schema->GetModelNames();

  std::for_each(model_names.begin(), model_names.end(), [&](ModelName model_name) {
    const ModelConfig& model = schema->model_config(model_name);
    auto cur_ttm = topic_model_.get(model_name);

    if (cur_ttm.get() != nullptr) {
      auto reg_names = model.regularizer_name();
      auto reg_tau = model.regularizer_tau();
      auto new_ttm = std::make_shared<::artm::core::TopicModel>(*cur_ttm);

      for (auto reg_name_iterator = reg_names.begin(); reg_name_iterator != reg_names.end();
        reg_name_iterator++) {
        auto regularizer = schema->regularizer(reg_name_iterator->c_str());

        if (regularizer != nullptr) {
          auto tau_index = reg_name_iterator - reg_names.begin();
          double tau = reg_tau.Get(tau_index);

          bool retval = regularizer->RegularizePhi(new_ttm.get(), tau);
          if (!retval) {
            LOG(ERROR) << "Problems with type or number of parameters in Phi regularizer <" <<
              reg_name_iterator->c_str() <<
              ">. On this iteration this regularizer was turned off.\n";
          }
        } else {
          LOG(ERROR) << "Phi Regularizer with name <" <<
            reg_name_iterator->c_str() << "> does not exist.";
        }
      }
      topic_model_.set(model_name, new_ttm);
    }
  });
}

void Merger::ThreadFunction() {
  try {
    Helpers::SetThreadName(-1, "Merger thread");
    LOG(INFO) << "Merger thread started";
    for (;;) {
      if (is_stopping) {
        break;
      }

      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      for (;;) {  // MAIN FOR LOOP
        // First, handle priority tasks in the internal_task_queue.
        MergerTask merger_task;
        if (internal_task_queue_.try_pop(&merger_task)) {
          switch (merger_task.task_type) {
            case kDisposeModel:
              new_topic_model_.erase(merger_task.model_name);
              break;
            case kForceSyncWithMemcached:
              SyncWithMemcached(merger_task.model_name);
              break;
            case kForceResetScores:
              ResetScores(merger_task.model_name);
          }

          if (merger_task.sync_event != nullptr) {
            merger_task.sync_event->signal();
          }

          continue;  // MAIN FOR LOOP
        }

        // Second, merge everything from the queue and update topic model.
        std::shared_ptr<const ProcessorOutput> processor_output;
        {
          boost::lock_guard<boost::mutex> guard(*merger_queue_lock_);
          if (merger_queue_->empty()) {
            break;  // MAIN FOR LOOP
          }

          processor_output = merger_queue_->front();
          merger_queue_->pop();
        }

        call_on_destruction c([&]() {
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
          ModelName model_name = model_increment.model_name();
          auto cur_ttm = topic_model_.get(model_name);
          if (cur_ttm.get() == nullptr) {
            // model had been disposed during ongoing processing;
            continue;  // for (int modex_index = 0; ...
          }

          auto iter = new_topic_model_.find(model_name);
          if (iter == new_topic_model_.end()) {
            new_topic_model_.insert(std::make_pair(
              model_name, std::make_shared<::artm::core::TopicModel>(*cur_ttm)));
            iter = new_topic_model_.find(model_name);
          }

          iter->second->ApplyDiff(model_increment);
        }
      }  // MAIN FOR LOOP
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

void Merger::SyncWithMemcached(ModelName model_name) {
  std::vector<ModelName> model_names;
  if (model_name.empty()) {
    for (auto iter = new_topic_model_.begin(); iter != new_topic_model_.end(); ++iter) {
      model_names.push_back(iter->first);
    }
  }

  for (auto &model_name : model_names) {
    // This method (SyncWithMemcached) calculates a diff between new_ttm and old_ttm,
    // and then sends it to MemcachedService as ModelIncrement.
    auto old_ttm = topic_model_.get(model_name);
    if (old_ttm.get() == nullptr)
      return;  // model had been disposed during ongoing processing;

    auto new_ttm = new_topic_model_.find(model_name);
    if (new_ttm == new_topic_model_.end())
      return;  // model had been disposed during ongoing processing;

    std::shared_ptr<MasterComponentService_Stub> master_component_service = master_component_service_->get();
    if (master_component_service == nullptr) {
      topic_model_.set(model_name, new_ttm->second);
      new_topic_model_.erase(model_name);
    } else {
      ModelIncrement model_increment;
      new_ttm->second->CalculateDiff(*old_ttm, &model_increment);

      try {
        ::artm::TopicModel reply;
        master_component_service->UpdateModel(model_increment, &reply);
        std::shared_ptr<::artm::core::TopicModel> new_global_ttm(
          new ::artm::core::TopicModel(reply));

        topic_model_.set(model_name, new_global_ttm);
        new_topic_model_.erase(model_name);
      } catch(const rpcz::rpc_error&) {
        LOG(ERROR) << "Merger failed to send updates to master component service.";
        throw;
      }
    }
  }
}

void Merger::ResetScores(ModelName model_name) {
  std::vector<ModelName> model_names;
  if (model_name.empty()) {
    model_names = topic_model_.keys();
  }

  std::shared_ptr<MasterComponentService_Stub> master_component_service = master_component_service_->get();
  for (auto &model_name : model_names) {
    if (master_component_service == nullptr) {
      auto old_ttm = GetLatestTopicModel(model_name);
      if (old_ttm == nullptr)
        return;  // model had been disposed during ongoing processing;

      auto new_ttm = std::make_shared<::artm::core::TopicModel>(*old_ttm);
      for (int score_index = 0; score_index < new_ttm->score_size(); ++score_index) {
        new_ttm->SetScores(score_index, 0.0, 0.0);
      }

      topic_model_.set(model_name, new_ttm);
    } else {
      // TODO(alfrey) to this on master
      std::string message("In Network mode ResetScores should happen on Master.");
      LOG(WARNING) << message;
    }
  }
}

}  // namespace core
}  // namespace artm

