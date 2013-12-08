#include "partition.h"

namespace topicmd {

void Partition::Add(const Batch& batch) {
  // merge dictionary
  std::map<int, int> renum;
  for (int i = 0; i < batch.token_size(); ++i) {
    std::string token = batch.token(i);
    auto iter = token_to_index_map_.find(token);
    if (iter != token_to_index_map_.end()) {
      renum.insert(std::make_pair(i, iter->second));
    } else {
      int new_id = index_to_token_map_.size();
      index_to_token_map_.push_back(token);
      token_to_index_map_.insert(std::make_pair(token, new_id));
      renum.insert(std::make_pair(i, new_id));
    }
  }
    
  for (int i = 0; i < batch.item_size(); ++i) {
    const Item& item = batch.item(i);
    std::unique_ptr<PartitionItem> partition_item(
        new PartitionItem());
    for (int iField = 0; iField < item.field_size(); ++iField) {
      const Field& field = item.field(iField);
      int token_size = field.token_id_size();
      for (int iToken = 0; iToken < token_size; ++iToken) {
        partition_item->AddToken(
            renum[field.token_id(iToken)],
            field.token_count(iToken));
      }
    }

    items_.push_back(std::move(partition_item));
  }
}

} // namespace topicmd
