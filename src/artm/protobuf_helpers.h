#ifndef ARTM_PROTOBUF_HELPERS_
#define ARTM_PROTOBUF_HELPERS_
 
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
bool repeated_field_index_of(const T& field, V value) 
{
  for (int i = 0; i < field.size(); ++i) {
    if (field.Get(i) == value) {
      return i;
    }
  }

  return -1;
}


}} // namespace artm::core

#endif // ARTM_PROTOBUF_HELPERS_