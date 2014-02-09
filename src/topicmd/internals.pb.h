// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: internals.proto

#ifndef PROTOBUF_internals_2eproto__INCLUDED
#define PROTOBUF_internals_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace artm {
namespace core {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_internals_2eproto();
void protobuf_AssignDesc_internals_2eproto();
void protobuf_ShutdownFile_internals_2eproto();

class Counters;
class ProcessorOutput;

// ===================================================================

class Counters : public ::google::protobuf::Message {
 public:
  Counters();
  virtual ~Counters();

  Counters(const Counters& from);

  inline Counters& operator=(const Counters& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Counters& default_instance();

  void Swap(Counters* other);

  // implements Message ----------------------------------------------

  Counters* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Counters& from);
  void MergeFrom(const Counters& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated float value = 1 [packed = true];
  inline int value_size() const;
  inline void clear_value();
  static const int kValueFieldNumber = 1;
  inline float value(int index) const;
  inline void set_value(int index, float value);
  inline void add_value(float value);
  inline const ::google::protobuf::RepeatedField< float >&
      value() const;
  inline ::google::protobuf::RepeatedField< float >*
      mutable_value();

  // @@protoc_insertion_point(class_scope:artm.core.Counters)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< float > value_;
  mutable int _value_cached_byte_size_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_internals_2eproto();
  friend void protobuf_AssignDesc_internals_2eproto();
  friend void protobuf_ShutdownFile_internals_2eproto();

  void InitAsDefaultInstance();
  static Counters* default_instance_;
};
// -------------------------------------------------------------------

class ProcessorOutput : public ::google::protobuf::Message {
 public:
  ProcessorOutput();
  virtual ~ProcessorOutput();

  ProcessorOutput(const ProcessorOutput& from);

  inline ProcessorOutput& operator=(const ProcessorOutput& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ProcessorOutput& default_instance();

  void Swap(ProcessorOutput* other);

  // implements Message ----------------------------------------------

  ProcessorOutput* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ProcessorOutput& from);
  void MergeFrom(const ProcessorOutput& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 model_id = 1;
  inline bool has_model_id() const;
  inline void clear_model_id();
  static const int kModelIdFieldNumber = 1;
  inline ::google::protobuf::int32 model_id() const;
  inline void set_model_id(::google::protobuf::int32 value);

  // required int32 topics_count = 2;
  inline bool has_topics_count() const;
  inline void clear_topics_count();
  static const int kTopicsCountFieldNumber = 2;
  inline ::google::protobuf::int32 topics_count() const;
  inline void set_topics_count(::google::protobuf::int32 value);

  // optional int32 items_processed = 3;
  inline bool has_items_processed() const;
  inline void clear_items_processed();
  static const int kItemsProcessedFieldNumber = 3;
  inline ::google::protobuf::int32 items_processed() const;
  inline void set_items_processed(::google::protobuf::int32 value);

  // required .artm.core.Counters topic_counters = 4;
  inline bool has_topic_counters() const;
  inline void clear_topic_counters();
  static const int kTopicCountersFieldNumber = 4;
  inline const ::artm::core::Counters& topic_counters() const;
  inline ::artm::core::Counters* mutable_topic_counters();
  inline ::artm::core::Counters* release_topic_counters();
  inline void set_allocated_topic_counters(::artm::core::Counters* topic_counters);

  // repeated .artm.core.Counters token_counters = 5;
  inline int token_counters_size() const;
  inline void clear_token_counters();
  static const int kTokenCountersFieldNumber = 5;
  inline const ::artm::core::Counters& token_counters(int index) const;
  inline ::artm::core::Counters* mutable_token_counters(int index);
  inline ::artm::core::Counters* add_token_counters();
  inline const ::google::protobuf::RepeatedPtrField< ::artm::core::Counters >&
      token_counters() const;
  inline ::google::protobuf::RepeatedPtrField< ::artm::core::Counters >*
      mutable_token_counters();

  // repeated string token = 6;
  inline int token_size() const;
  inline void clear_token();
  static const int kTokenFieldNumber = 6;
  inline const ::std::string& token(int index) const;
  inline ::std::string* mutable_token(int index);
  inline void set_token(int index, const ::std::string& value);
  inline void set_token(int index, const char* value);
  inline void set_token(int index, const char* value, size_t size);
  inline ::std::string* add_token();
  inline void add_token(const ::std::string& value);
  inline void add_token(const char* value);
  inline void add_token(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& token() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_token();

  // repeated string discovered_token = 7;
  inline int discovered_token_size() const;
  inline void clear_discovered_token();
  static const int kDiscoveredTokenFieldNumber = 7;
  inline const ::std::string& discovered_token(int index) const;
  inline ::std::string* mutable_discovered_token(int index);
  inline void set_discovered_token(int index, const ::std::string& value);
  inline void set_discovered_token(int index, const char* value);
  inline void set_discovered_token(int index, const char* value, size_t size);
  inline ::std::string* add_discovered_token();
  inline void add_discovered_token(const ::std::string& value);
  inline void add_discovered_token(const char* value);
  inline void add_discovered_token(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& discovered_token() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_discovered_token();

  // @@protoc_insertion_point(class_scope:artm.core.ProcessorOutput)
 private:
  inline void set_has_model_id();
  inline void clear_has_model_id();
  inline void set_has_topics_count();
  inline void clear_has_topics_count();
  inline void set_has_items_processed();
  inline void clear_has_items_processed();
  inline void set_has_topic_counters();
  inline void clear_has_topic_counters();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 model_id_;
  ::google::protobuf::int32 topics_count_;
  ::artm::core::Counters* topic_counters_;
  ::google::protobuf::RepeatedPtrField< ::artm::core::Counters > token_counters_;
  ::google::protobuf::RepeatedPtrField< ::std::string> token_;
  ::google::protobuf::RepeatedPtrField< ::std::string> discovered_token_;
  ::google::protobuf::int32 items_processed_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(7 + 31) / 32];

  friend void  protobuf_AddDesc_internals_2eproto();
  friend void protobuf_AssignDesc_internals_2eproto();
  friend void protobuf_ShutdownFile_internals_2eproto();

  void InitAsDefaultInstance();
  static ProcessorOutput* default_instance_;
};
// ===================================================================


// ===================================================================

// Counters

// repeated float value = 1 [packed = true];
inline int Counters::value_size() const {
  return value_.size();
}
inline void Counters::clear_value() {
  value_.Clear();
}
inline float Counters::value(int index) const {
  return value_.Get(index);
}
inline void Counters::set_value(int index, float value) {
  value_.Set(index, value);
}
inline void Counters::add_value(float value) {
  value_.Add(value);
}
inline const ::google::protobuf::RepeatedField< float >&
Counters::value() const {
  return value_;
}
inline ::google::protobuf::RepeatedField< float >*
Counters::mutable_value() {
  return &value_;
}

// -------------------------------------------------------------------

// ProcessorOutput

// required int32 model_id = 1;
inline bool ProcessorOutput::has_model_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ProcessorOutput::set_has_model_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ProcessorOutput::clear_has_model_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ProcessorOutput::clear_model_id() {
  model_id_ = 0;
  clear_has_model_id();
}
inline ::google::protobuf::int32 ProcessorOutput::model_id() const {
  return model_id_;
}
inline void ProcessorOutput::set_model_id(::google::protobuf::int32 value) {
  set_has_model_id();
  model_id_ = value;
}

// required int32 topics_count = 2;
inline bool ProcessorOutput::has_topics_count() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ProcessorOutput::set_has_topics_count() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ProcessorOutput::clear_has_topics_count() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ProcessorOutput::clear_topics_count() {
  topics_count_ = 0;
  clear_has_topics_count();
}
inline ::google::protobuf::int32 ProcessorOutput::topics_count() const {
  return topics_count_;
}
inline void ProcessorOutput::set_topics_count(::google::protobuf::int32 value) {
  set_has_topics_count();
  topics_count_ = value;
}

// optional int32 items_processed = 3;
inline bool ProcessorOutput::has_items_processed() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ProcessorOutput::set_has_items_processed() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ProcessorOutput::clear_has_items_processed() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ProcessorOutput::clear_items_processed() {
  items_processed_ = 0;
  clear_has_items_processed();
}
inline ::google::protobuf::int32 ProcessorOutput::items_processed() const {
  return items_processed_;
}
inline void ProcessorOutput::set_items_processed(::google::protobuf::int32 value) {
  set_has_items_processed();
  items_processed_ = value;
}

// required .artm.core.Counters topic_counters = 4;
inline bool ProcessorOutput::has_topic_counters() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ProcessorOutput::set_has_topic_counters() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ProcessorOutput::clear_has_topic_counters() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ProcessorOutput::clear_topic_counters() {
  if (topic_counters_ != NULL) topic_counters_->::artm::core::Counters::Clear();
  clear_has_topic_counters();
}
inline const ::artm::core::Counters& ProcessorOutput::topic_counters() const {
  return topic_counters_ != NULL ? *topic_counters_ : *default_instance_->topic_counters_;
}
inline ::artm::core::Counters* ProcessorOutput::mutable_topic_counters() {
  set_has_topic_counters();
  if (topic_counters_ == NULL) topic_counters_ = new ::artm::core::Counters;
  return topic_counters_;
}
inline ::artm::core::Counters* ProcessorOutput::release_topic_counters() {
  clear_has_topic_counters();
  ::artm::core::Counters* temp = topic_counters_;
  topic_counters_ = NULL;
  return temp;
}
inline void ProcessorOutput::set_allocated_topic_counters(::artm::core::Counters* topic_counters) {
  delete topic_counters_;
  topic_counters_ = topic_counters;
  if (topic_counters) {
    set_has_topic_counters();
  } else {
    clear_has_topic_counters();
  }
}

// repeated .artm.core.Counters token_counters = 5;
inline int ProcessorOutput::token_counters_size() const {
  return token_counters_.size();
}
inline void ProcessorOutput::clear_token_counters() {
  token_counters_.Clear();
}
inline const ::artm::core::Counters& ProcessorOutput::token_counters(int index) const {
  return token_counters_.Get(index);
}
inline ::artm::core::Counters* ProcessorOutput::mutable_token_counters(int index) {
  return token_counters_.Mutable(index);
}
inline ::artm::core::Counters* ProcessorOutput::add_token_counters() {
  return token_counters_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::artm::core::Counters >&
ProcessorOutput::token_counters() const {
  return token_counters_;
}
inline ::google::protobuf::RepeatedPtrField< ::artm::core::Counters >*
ProcessorOutput::mutable_token_counters() {
  return &token_counters_;
}

// repeated string token = 6;
inline int ProcessorOutput::token_size() const {
  return token_.size();
}
inline void ProcessorOutput::clear_token() {
  token_.Clear();
}
inline const ::std::string& ProcessorOutput::token(int index) const {
  return token_.Get(index);
}
inline ::std::string* ProcessorOutput::mutable_token(int index) {
  return token_.Mutable(index);
}
inline void ProcessorOutput::set_token(int index, const ::std::string& value) {
  token_.Mutable(index)->assign(value);
}
inline void ProcessorOutput::set_token(int index, const char* value) {
  token_.Mutable(index)->assign(value);
}
inline void ProcessorOutput::set_token(int index, const char* value, size_t size) {
  token_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ProcessorOutput::add_token() {
  return token_.Add();
}
inline void ProcessorOutput::add_token(const ::std::string& value) {
  token_.Add()->assign(value);
}
inline void ProcessorOutput::add_token(const char* value) {
  token_.Add()->assign(value);
}
inline void ProcessorOutput::add_token(const char* value, size_t size) {
  token_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
ProcessorOutput::token() const {
  return token_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
ProcessorOutput::mutable_token() {
  return &token_;
}

// repeated string discovered_token = 7;
inline int ProcessorOutput::discovered_token_size() const {
  return discovered_token_.size();
}
inline void ProcessorOutput::clear_discovered_token() {
  discovered_token_.Clear();
}
inline const ::std::string& ProcessorOutput::discovered_token(int index) const {
  return discovered_token_.Get(index);
}
inline ::std::string* ProcessorOutput::mutable_discovered_token(int index) {
  return discovered_token_.Mutable(index);
}
inline void ProcessorOutput::set_discovered_token(int index, const ::std::string& value) {
  discovered_token_.Mutable(index)->assign(value);
}
inline void ProcessorOutput::set_discovered_token(int index, const char* value) {
  discovered_token_.Mutable(index)->assign(value);
}
inline void ProcessorOutput::set_discovered_token(int index, const char* value, size_t size) {
  discovered_token_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ProcessorOutput::add_discovered_token() {
  return discovered_token_.Add();
}
inline void ProcessorOutput::add_discovered_token(const ::std::string& value) {
  discovered_token_.Add()->assign(value);
}
inline void ProcessorOutput::add_discovered_token(const char* value) {
  discovered_token_.Add()->assign(value);
}
inline void ProcessorOutput::add_discovered_token(const char* value, size_t size) {
  discovered_token_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
ProcessorOutput::discovered_token() const {
  return discovered_token_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
ProcessorOutput::mutable_discovered_token() {
  return &discovered_token_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace core
}  // namespace artm

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_internals_2eproto__INCLUDED
