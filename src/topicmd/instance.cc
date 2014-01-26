#include "topicmd/instance.h"

#include <iostream>

#include <boost/bind.hpp>

#include "topicmd/common.h"
#include "topicmd/processor.h"

namespace topicmd {
  Instance::Instance(int id, const InstanceConfig& config) :
    lock_(),
    instance_id_(id),
    next_generation_id_(1),
    current_partition_(std::make_shared<Partition>()),
    finished_partition_(),
    published_generation_(lock_),
    schema_(lock_, std::make_shared<InstanceSchema>(InstanceSchema(config))),
    processor_queue_lock_(),
    processor_queue_(),
    merger_queue_lock_(),
    merger_queue_(),
    data_loader_(processor_queue_lock_, processor_queue_, published_generation_),
    merger_(merger_queue_lock_, merger_queue_, published_generation_, schema_),
    processors_()
  {
    Reconfigure(config);
  }

  Instance::~Instance() {
  }

  int Instance::UpdateModel(int model_id, const ModelConfig& config) {
    auto new_schema = schema_.get_copy();
    new_schema->set_model_config(model_id, std::make_shared<const ModelConfig>(config));
    schema_.set(new_schema);
    return TOPICMD_SUCCESS;
  }

  int Instance::DisposeModel(int model_id) {
    auto new_schema = schema_.get_copy();
    new_schema->discard_model(model_id);
    schema_.set(new_schema);
    return TOPICMD_SUCCESS;
  }

  int Instance::DiscardPartition() {
    current_partition_ = std::make_shared<Partition>();
    return TOPICMD_SUCCESS;
  }

  int Instance::GetTotalItemsCount() const {
    auto latest = published_generation_.get();
    return latest->GetTotalItemsCount();
  }

  int Instance::FinishPartition() {
    int last_id = next_generation_id_++;
    finished_partition_.insert(
      std::pair<int, std::shared_ptr<const Partition> >(
        last_id, current_partition_));

    current_partition_ = std::make_shared<Partition>();
    return last_id;
  }

  int Instance::InsertBatch(const Batch& batch) {
    current_partition_->Add(batch);
    return TOPICMD_SUCCESS;
  }

  int Instance::PublishGeneration(int generation_id) 
  {
    auto next_generation_ = published_generation_.get_copy();

    // move all partitions up to generation_id
    // from finished_partition_ to published_generation_.
    for (auto iter = finished_partition_.begin();
      iter != finished_partition_.end(); 
      /* nothing */) 
    {
      if ((*iter).first <= generation_id) {
        next_generation_->AddPartition(iter->second);
        finished_partition_.erase(iter++);
      }
      else {
        ++iter;
      }
    }

    published_generation_.set(next_generation_);
    return TOPICMD_SUCCESS;
  }

  int Instance::Reconfigure(const InstanceConfig& config) {
    auto new_schema = schema_.get_copy();
    new_schema->set_instance_config(config);
    schema_.set(new_schema);

    // Adjust size of processors_ 
    while (processors_.size() > config.processors_count()) processors_.pop_back();
    while (processors_.size() < config.processors_count()) 
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

    return TOPICMD_SUCCESS;
  }

  int Instance::RequestModelTopics(int model_id, ModelTopics* model_topics) {
    std::shared_ptr<const TokenTopicMatrix> ttm = merger_.token_topic_matrix(model_id);
    int nTopics = ttm->topics_count(); 
    for (int iToken = 0; iToken < ttm->tokens_count(); iToken++) {
      TokenTopics* token_topics = model_topics->add_token_topic();
      token_topics->set_token(ttm->token(iToken));
      float* token_topics_weight = ttm->token_topics(iToken);
      for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
        token_topics->add_topic_weight(token_topics_weight[iTopic]);
      }
    }
    
    return TOPICMD_SUCCESS;
  }

  int Instance::WaitModelProcessed(int model_id, int processed_items) {
    for (;;) {
      std::shared_ptr<const TokenTopicMatrix> ttm = merger_.token_topic_matrix(model_id);
      if (ttm->items_processed() >= processed_items) {
        return TOPICMD_SUCCESS;
      }

      boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
  }
} // namespace topicmd

