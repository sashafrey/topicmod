#ifndef ARTM_PROTOBUF_HELPERS_
#define ARTM_PROTOBUF_HELPERS_

#include <string>

#include "artm/messages.pb.h"

namespace artm { namespace core {

template<class T, class V>
bool repeated_field_contains(const T& field, V value) 
{
  for (int i = 0; i < field.size(); ++i) {
    if (field.Get(i) == value) {
      return true;
    }
  }

  return false;
}

template<class T, class V>
int repeated_field_index_of(const T& field, V value) 
{
  for (int i = 0; i < field.size(); ++i) {
    if (field.Get(i) == value) {
      return i;
    }
  }

  return -1;
}

inline bool model_has_token(const ModelTopics& model_topics, std::string token) {
  for (int i = 0; i < model_topics.token_topic_size(); ++i) {
    std::string cur_token = model_topics.token_topic(i).token();
    if (cur_token == token) return true;
  }

  return false;
}

}} // namespace artm::core

#endif // ARTM_PROTOBUF_HELPERS_