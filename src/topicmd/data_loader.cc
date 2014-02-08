#include "data_loader.h"

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

      {
        boost::lock_guard<boost::mutex> guard(lock_);
        if (queue_.size() > 10) {
          continue; 
        }
      }

      {
        auto latest_generation = generation_.get();

        boost::lock_guard<boost::mutex> guard(lock_);
        latest_generation->InvokeOnEachPartition([&](std::shared_ptr<const Partition> part) { 
          queue_.push(part);
        });
      }
    }
  }
  catch(boost::thread_interrupted&) {
    return;
  }
}

} // namespace topicmd