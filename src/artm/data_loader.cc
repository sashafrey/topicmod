#include "artm/data_loader.h"
#include "artm/protobuf_helpers.h"

namespace artm { namespace core {

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

int DataLoader::Reconfigure(const DataLoaderConfig& config) {
  config_.set(std::make_shared<DataLoaderConfig>(config));
  return ARTM_SUCCESS;
}

int DataLoader::AddBatch(const Batch& batch)  
{
  std::shared_ptr<Generation> next_gen = generation_.get_copy();
  next_gen->AddBatch(std::make_shared<Batch>(batch));
  generation_.set(next_gen);
  return ARTM_SUCCESS;
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
      if (!InstanceManager::singleton().Contains(config->instance_id())) {
        continue;
      }

      auto instance = InstanceManager::singleton().Get(config->instance_id());
      if (instance->ProcessorQueueSize() >= config->queue_size()) {
        continue;
      }

      {
        auto latest_generation = generation_.get();

        boost::lock_guard<boost::mutex> guard(lock_);
        latest_generation->InvokeOnEachPartition([&](std::shared_ptr<const Batch> batch) { 
          auto pi = std::make_shared<ProcessorInput>();
          pi->mutable_batch()->CopyFrom(*batch);
          
          // loop through all streams
          for (int iStream = 0; iStream < config->stream_size(); ++iStream) {
            const Stream& stream = config->stream(iStream);
            pi->add_stream_name(stream.name());
            
            Flags* flags = pi->add_stream_flags();
            for (int iItem = 0; iItem < batch->item_size(); ++iItem) {
              // verify if item is part of the stream
              bool value = false;
              switch (stream.type()) 
              {
                case Stream_Type_Global:
                {
                  value = true; 
                  break; // Stream_Type_Global
                }

                case Stream_Type_ItemIdModulus: 
                {
                  int id_mod = batch->item(iItem).id() % stream.modulus();
                  value = repeated_field_contains(stream.residuals(), id_mod);
                  break; // Stream_Type_ItemIdModulus
                }
                
                case Stream_Type_ItemHashModulus:
                default:
                  throw "bad santa"; // not implemented.
              }

              flags->add_value(true);
            }
          }
          
          instance->AddBatchIntoProcessorQueue(pi);
        });
      }
    }
  }
  catch(boost::thread_interrupted&) {
    return;
  }
}

}} // namespace artm/core