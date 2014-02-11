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

  private:
    friend class TemplateManager<DataLoader, DataLoaderConfig>;

    // All instances of DataLoader should be created via DataLoader::Manager
    DataLoader(int id, const DataLoaderConfig& config) :
      lock_(),
      config_(lock_, std::make_shared<DataLoaderConfig>(config)),
      generation_(lock_, std::make_shared<Generation>()),
      thread_(boost::bind(&DataLoader::ThreadFunction, this))
    {
    }

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