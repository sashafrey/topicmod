#ifndef ARTM_GENERATION_
#define ARTM_GENERATION_

#include <map>
#include <memory>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include "messages.pb.h"

namespace artm { namespace core {
  class Generation {
  private:
    int id_;
    std::map<boost::uuids::uuid, std::shared_ptr<const Batch> > generation_;
  public:
    Generation() : id_(0), generation_() 
    {
    }

    Generation(const Generation& generation) : 
        id_(generation.id_ + 1), 
        generation_(generation.generation_)
    {
    }

    int get_id() const {
      return id_;
    }
    
    std::shared_ptr<const Batch> batch(const boost::uuids::uuid& uuid) {
      auto retval = generation_.find(uuid);
      return (retval != generation_.end()) ? retval->second : nullptr;
    }

    void AddBatch(const std::shared_ptr<const Batch>& batch) 
    {
      generation_.insert(std::make_pair(boost::uuids::random_generator()(), batch));
    }

    template<class Function>
    void InvokeOnEachPartition(Function fn) const {
      for (auto iter = generation_.begin();
           iter != generation_.end();
           ++iter) 
      {
        fn(iter->first, iter->second);
      }
    }

    int GetTotalItemsCount() const {
      int retval = 0;
      for (auto iter = generation_.begin();
           iter != generation_.end();
           ++iter) 
      {
        retval += (*iter).second->item_size();
      }

      return retval;
    }
  };
}} // namespace artm/core

#endif // ARTM_GENERATION_
