#ifndef TOPICMD_GENERATION_
#define TOPICMD_GENERATION_

#include <map>
#include <memory>

#include <boost/uuid/uuid.hpp>            // uuid class

#include "topicmd/partition.h"

namespace topicmd {
  class Generation {
  private:
    std::map<boost::uuids::uuid, std::shared_ptr<const Partition> > generation_;
  public:
    void AddPartition(const std::shared_ptr<const Partition>& partition) 
    {
      generation_.insert(std::make_pair(partition->uuid(), partition));
    }

    template<class Function>
    void InvokeOnEachPartition(Function fn) const {
      for (auto iter = generation_.begin();
	   iter != generation_.end();
	   ++iter) {
	fn(iter->second);
      }
    }

    int GetTotalItemsCount() const {
      int retval = 0;
      for (auto iter = generation_.begin();
	   iter != generation_.end();
	   ++iter) {
	retval += (*iter).second->GetItemsCount();
      }

      return retval;
    }
  };
}

#endif // TOPICMD_GENERATION_
