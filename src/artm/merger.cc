#include "merger.h"

#include <algorithm>
#include <iostream>

#include "artm/call_on_destruction.h"
#include "artm/data_loader.h"

namespace artm {
namespace core {

Merger::Merger(boost::mutex& merger_queue_lock,
               std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
               ThreadSafeHolder<InstanceSchema>& schema) :
    lock_(),
    token_topic_matrix_(lock_),
    schema_(schema),
    merger_queue_lock_(merger_queue_lock),
    merger_queue_(merger_queue),
    thread_()
{
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

void Merger::DisposeModel(int model_id) {
  token_topic_matrix_.erase(model_id);
}

void Merger::UpdateModel(int model_id, const ModelConfig& model) {
  if (!token_topic_matrix_.has_key(model_id)) {
    // Handle more type of reconfigs - for example, changing the number of topics;
    auto ttm = std::make_shared<TokenTopicMatrix>(model.topics_count(), model.score_size());
    token_topic_matrix_.set(model_id, ttm);
  }

  auto ttm = token_topic_matrix_.get(model_id);
  if (ttm->topics_count() != model.topics_count()) {
    throw "Unsupported reconfiguration";
  }
}

std::shared_ptr<const TokenTopicMatrix>
Merger::GetLatestTokenTopicMatrix(int model_id) const
{
  return token_topic_matrix_.get(model_id);
}

void Merger::ThreadFunction()
{
  try {
    for (;;)
    {
      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      // Merge everything from the queue and update matrix.
      for (;;) {
        std::shared_ptr<const ProcessorOutput> processor_output;
        {
          boost::lock_guard<boost::mutex> guard(merger_queue_lock_);
          if (merger_queue_.empty()) {
            break;
          }

          processor_output = merger_queue_.front();
          merger_queue_.pop();
        }

        helpers::call_on_destruction c([&]() {
          // Callback to DataLoader
          auto data_loader = DataLoaderManager::singleton().Get(
            processor_output->data_loader_id());

          if (data_loader != nullptr) {
            data_loader->Callback(processor_output);
          }
        });

        for (int iModel = 0; iModel < processor_output->model_increment_size(); iModel++) {
          auto model_increment = processor_output->model_increment(iModel);
          int model_id = model_increment.model_id();
          auto cur_ttm = token_topic_matrix_.get(model_id);
          if (cur_ttm.get() == nullptr) {
            // a model had been disposed during ongoing processing;
            continue;
          }
       
          auto new_ttm = std::make_shared<TokenTopicMatrix>(*cur_ttm);
          new_ttm->IncreaseItemsProcessed(model_increment.items_processed());
          for (int iScore = 0; iScore < model_increment.score_size(); ++iScore) {
            new_ttm->IncreaseScores(
              iScore, model_increment.score(iScore), model_increment.score_norm(iScore));
          }

          // Add new tokens discovered by processor
          for (int iToken = 0; iToken < model_increment.discovered_token_size(); ++iToken) {
            std::string new_token = model_increment.discovered_token(iToken);
            if (new_ttm->token_id(new_token) == -1) {
              new_ttm->AddToken(new_token);
            }
          }

          int topics_count = new_ttm->topics_count();

          for (int iToken = 0; iToken < model_increment.token_increment_size(); ++iToken) {
            const FloatArray& counters = model_increment.token_increment(iToken);
            const std::string& token = model_increment.token(iToken);
            int id = new_ttm->token_id(token);
            for (int iTopic = 0; iTopic < topics_count; ++iTopic) {
              new_ttm->IncreaseTokenWeight(id, iTopic, counters.value(iTopic));
            }
          }

          {
            boost::lock_guard<boost::mutex> guard(merger_queue_lock_);
            // new_ttm->ToString();
          }
          token_topic_matrix_.set(model_id, new_ttm);
        }
      }
    }
  }
  catch(boost::thread_interrupted&) {
    return;
  }
}

}} // namespace artm/core
