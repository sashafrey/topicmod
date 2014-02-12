#ifndef DATA_LOADER_
#define DATA_LOADER_

#include <queue>

#include <boost/thread.hpp>   
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "artm/common.h"
#include "artm/generation.h"
#include "artm/messages.pb.h"
#include "artm/instance.h"
#include "artm/template_manager.h"
#include "artm/thread_safe_holder.h"

namespace artm { namespace core {
  class DataLoader : boost::noncopyable {
  public:
    ~DataLoader();
    void Interrupt();
    void Join();
    int AddBatch(const Batch& batch);

    int GetTotalItemsCount() const {
      auto ptr = generation_.get();
      return ptr->GetTotalItemsCount();
    }

    int Reconfigure(const DataLoaderConfig& config);

    int InvokeIteration(int iterations_count) {
      if (iterations_count <= 0) return ARTM_ERROR;
      boost::lock_guard<boost::mutex> guard(lock_);
      pending_iterations_count_ += iterations_count;
      return ARTM_SUCCESS;
    }

    int WaitIdle() {
      for (;;) 
      {
        {
          boost::lock_guard<boost::mutex> guard(lock_);
          if (pending_iterations_count_ <= 0) return ARTM_SUCCESS;
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
      }
    }

    int id() const {
      return data_loader_id_;
    }

  private:
    friend class TemplateManager<DataLoader, DataLoaderConfig>;

    // All instances of DataLoader should be created via DataLoaderManager
    DataLoader(int id, const DataLoaderConfig& config);

    int data_loader_id_;
    int pending_iterations_count_;
    boost::mutex lock_;
    ThreadSafeHolder<DataLoaderConfig> config_;
    ThreadSafeHolder<Generation> generation_;

    // Keep all threads at the end of class members
    // (because the order of class members defines initialization order;
    // everything else should be initialized before creating threads).
    boost::thread thread_;

    void ThreadFunction();
  };

  typedef TemplateManager<DataLoader, DataLoaderConfig> DataLoaderManager;
}} // namespace artm/core

#endif // DATA_LOADER_