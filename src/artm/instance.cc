#include "artm/instance.h"

#include <iostream>

#include <boost/bind.hpp>

#include "artm/common.h"
#include "artm/processor.h"
#include "artm/template_manager.h"

namespace artm { namespace core {
  Instance::Instance(int id, const InstanceConfig& config) :
    lock_(),
    instance_id_(id),
    schema_(lock_, std::make_shared<InstanceSchema>(InstanceSchema(config))),
    next_model_id_(0),
    processor_queue_lock_(),
    processor_queue_(),
    merger_queue_lock_(),
    merger_queue_(),
    merger_(merger_queue_lock_, merger_queue_, schema_),
    processors_()
  {
    Reconfigure(config);
  }

  Instance::~Instance() {
  }

  int Instance::CreateModel(const ModelConfig& config) {
    int model_id = next_model_id_++;
    ReconfigureModel(model_id, config);
    return model_id;
  }

  int Instance::ReconfigureModel(int model_id, const ModelConfig& config) {
    merger_.UpdateModel(model_id, config);
    
    auto new_schema = schema_.get_copy();
    new_schema->set_model_config(model_id, std::make_shared<const ModelConfig>(config));
    schema_.set(new_schema);
    return ARTM_SUCCESS;
  }

  int Instance::DisposeModel(int model_id) {
    auto new_schema = schema_.get_copy();
    new_schema->discard_model(model_id);
    schema_.set(new_schema);

    merger_.DisposeModel(model_id);
    return ARTM_SUCCESS;
  }

  int Instance::Reconfigure(const InstanceConfig& config) {
    auto new_schema = schema_.get_copy();
    new_schema->set_instance_config(config);
    schema_.set(new_schema);

    // Adjust size of processors_ 
    while ((int)processors_.size() > config.processors_count()) processors_.pop_back();
    while ((int)processors_.size() < config.processors_count()) 
    {
      processors_.push_back(
        std::shared_ptr<Processor>(new Processor(
          processor_queue_lock_, 
          processor_queue_, 
          merger_queue_lock_, 
          merger_queue_, 
          merger_, 
          schema_)));
    }

    return ARTM_SUCCESS;
  }

  int Instance::RequestModelTopics(int model_id, ModelTopics* model_topics) {
    std::shared_ptr<const TokenTopicMatrix> ttm = merger_.GetLatestTokenTopicMatrix(model_id);
    int nTopics = ttm->topics_count(); 
    for (int iToken = 0; iToken < ttm->tokens_count(); iToken++) {
      TokenTopics* token_topics = model_topics->add_token_topic();
      token_topics->set_token(ttm->token(iToken));
      TokenWeights token_weights = ttm->token_weights(iToken);
      for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
        token_topics->add_topic_weight(token_weights.at(iTopic));
      }

      model_topics->set_items_processed(ttm->items_processed());
    }
    
    return ARTM_SUCCESS;
  }

  int Instance::WaitModelProcessed(int model_id, int processed_items) {
    for (;;) {
      std::shared_ptr<const TokenTopicMatrix> ttm = merger_.GetLatestTokenTopicMatrix(model_id);
      if (ttm->items_processed() >= processed_items) {
        return ARTM_SUCCESS;
      }

      boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
  }

  int Instance::WaitIdle() {
    for (;;) {
      bool retval = true;
      {
        boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
        if (!processor_queue_.empty()) retval = false;
      }

      {
        boost::lock_guard<boost::mutex> guard(merger_queue_lock_);
        if (!merger_queue_.empty()) retval = false;
      }

      if (retval) {
        return true;
      }

      boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
  }

  int Instance::ProcessorQueueSize() {
    boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
    return processor_queue_.size();
  }

  int Instance::AddBatchIntoProcessorQueue(std::shared_ptr<const ProcessorInput> input) {
    boost::lock_guard<boost::mutex> guard(processor_queue_lock_);
    processor_queue_.push(input);
    return ARTM_SUCCESS;
  }
}} // namespace artm/core

