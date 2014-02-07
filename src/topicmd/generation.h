#ifndef TOPICMD_GENERATION_
#define TOPICMD_GENERATION_

#include <map>
#include <memory>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include "messages.pb.h"

namespace topicmd {
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
        fn(iter->second);
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
}

#endif // TOPICMD_GENERATION_
