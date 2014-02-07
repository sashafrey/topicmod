#include "topicmd/data_loader.h"

namespace topicmd {

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

int DataLoader::AddBatch(const Batch& batch)  
{
  std::shared_ptr<Generation> next_gen = generation_.get_copy();
  next_gen->AddBatch(std::make_shared<Batch>(batch));
  generation_.set(next_gen);
  return TOPICMD_SUCCESS;
}

void DataLoader::ThreadFunction() 
{
  try {
    for (;;)
    {
      // Sleep and check for interrupt.
      // To check for interrupt without sleep,
      // use boost::this_thread::interruption_point()
      // which also throws boost::thread_interrupted
      boost::this_thread::sleep(boost::posix_time::milliseconds(1));

      auto config = config_.get();
      if (!InstanceManager::singleton().has_instance(config->instance_id())) {
        continue;
      }

      auto instance = InstanceManager::singleton().instance(config->instance_id());
      if (instance->ProcessorQueueSize() >= config->queue_size()) {
        continue;
      }

      {
        auto latest_generation = generation_.get();

        boost::lock_guard<boost::mutex> guard(lock_);
        latest_generation->InvokeOnEachPartition([&](std::shared_ptr<const Batch> batch) { 
          instance->AddBatchIntoProcessorQueue(batch);
        });
      }
    }
  }
  catch(boost::thread_interrupted&) {
    return;
  }
}

} // namespace topicmd