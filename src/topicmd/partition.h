#ifndef TOPICMD_PARTITION_
#define TOPICMD_PARTITION_

#include <memory>
#include <vector>
#include <map>
#include <string>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/utility.hpp>

#include "topicmd/messages.pb.h"

namespace topicmd { 
class Partition : boost::noncopyable {
 private:
  class PartitionItem : boost::noncopyable {
   public:
    int token_size() const {
      return token_index_.size();
    }

    int token(int index) const {
      return token_index_[index];
    }

    int frequency(int index) const {
      return token_frequency_[index];
    }
    
    void AddToken(int token, int frequency) {
      token_index_.push_back(token);
      token_frequency_.push_back(frequency);
    }
   private:
    std::vector<int> token_index_;
    std::vector<int> token_frequency_;
  };
    
  boost::uuids::uuid uuid_;
  std::map<std::string, int> token_to_index_map_;
  std::vector<std::string> index_to_token_map_;
  std::vector<std::unique_ptr<PartitionItem> > items_;
 public:
  Partition()
      : uuid_(boost::uuids::random_generator()())
  {
  }
  
  void Add(const Batch& batch);
   
  boost::uuids::uuid uuid() const {
    return uuid_;
  }

  const std::vector<std::string>& get_tokens() const {
    return index_to_token_map_;
  }

  int get_item_count() const {
    return items_.size();
  }

  int get_token_count(int item_index) const {
    return items_[item_index]->token_size();
  }

  std::string get_token(int item_index, int token_index) const {
    int index = items_[item_index]->token(token_index);
    return index_to_token_map_[index];
  }

  int get_token_frequency(int item_index, int token_index) const {
    return items_[item_index]->frequency(token_index);
  }
        
  int GetItemsCount() const {
    return get_item_count();
  }
};
}

#endif // TOPICMD_PARTITION_
