// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: internals.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "internals.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace artm {
namespace core {

namespace {

const ::google::protobuf::Descriptor* Counters_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Counters_reflection_ = NULL;
const ::google::protobuf::Descriptor* ProcessorOutput_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ProcessorOutput_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_internals_2eproto() {
  protobuf_AddDesc_internals_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "internals.proto");
  GOOGLE_CHECK(file != NULL);
  Counters_descriptor_ = file->message_type(0);
  static const int Counters_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Counters, value_),
  };
  Counters_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Counters_descriptor_,
      Counters::default_instance_,
      Counters_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Counters, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Counters, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Counters));
  ProcessorOutput_descriptor_ = file->message_type(1);
  static const int ProcessorOutput_offsets_[7] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, model_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, topics_count_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, items_processed_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, topic_counters_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, token_counters_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, token_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, discovered_token_),
  };
  ProcessorOutput_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ProcessorOutput_descriptor_,
      ProcessorOutput::default_instance_,
      ProcessorOutput_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ProcessorOutput, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ProcessorOutput));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_internals_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Counters_descriptor_, &Counters::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ProcessorOutput_descriptor_, &ProcessorOutput::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_internals_2eproto() {
  delete Counters::default_instance_;
  delete Counters_reflection_;
  delete ProcessorOutput::default_instance_;
  delete ProcessorOutput_reflection_;
}

void protobuf_AddDesc_internals_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017internals.proto\022\tartm.core\"\035\n\010Counters"
    "\022\021\n\005value\030\001 \003(\002B\002\020\001\"\325\001\n\017ProcessorOutput\022"
    "\020\n\010model_id\030\001 \002(\005\022\024\n\014topics_count\030\002 \002(\005\022"
    "\027\n\017items_processed\030\003 \001(\005\022+\n\016topic_counte"
    "rs\030\004 \002(\0132\023.artm.core.Counters\022+\n\016token_c"
    "ounters\030\005 \003(\0132\023.artm.core.Counters\022\r\n\005to"
    "ken\030\006 \003(\t\022\030\n\020discovered_token\030\007 \003(\t", 275);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "internals.proto", &protobuf_RegisterTypes);
  Counters::default_instance_ = new Counters();
  ProcessorOutput::default_instance_ = new ProcessorOutput();
  Counters::default_instance_->InitAsDefaultInstance();
  ProcessorOutput::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_internals_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_internals_2eproto {
  StaticDescriptorInitializer_internals_2eproto() {
    protobuf_AddDesc_internals_2eproto();
  }
} static_descriptor_initializer_internals_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int Counters::kValueFieldNumber;
#endif  // !_MSC_VER

Counters::Counters()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Counters::InitAsDefaultInstance() {
}

Counters::Counters(const Counters& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Counters::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Counters::~Counters() {
  SharedDtor();
}

void Counters::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Counters::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Counters::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Counters_descriptor_;
}

const Counters& Counters::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_internals_2eproto();
  return *default_instance_;
}

Counters* Counters::default_instance_ = NULL;

Counters* Counters::New() const {
  return new Counters;
}

void Counters::Clear() {
  value_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Counters::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated float value = 1 [packed = true];
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, this->mutable_value())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_FIXED32) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitiveNoInline<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 1, 10, input, this->mutable_value())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Counters::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated float value = 1 [packed = true];
  if (this->value_size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteTag(1, ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED, output);
    output->WriteVarint32(_value_cached_byte_size_);
  }
  for (int i = 0; i < this->value_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteFloatNoTag(
      this->value(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Counters::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated float value = 1 [packed = true];
  if (this->value_size() > 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteTagToArray(
      1,
      ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED,
      target);
    target = ::google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(
      _value_cached_byte_size_, target);
  }
  for (int i = 0; i < this->value_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteFloatNoTagToArray(this->value(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Counters::ByteSize() const {
  int total_size = 0;

  // repeated float value = 1 [packed = true];
  {
    int data_size = 0;
    data_size = 4 * this->value_size();
    if (data_size > 0) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(data_size);
    }
    GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
    _value_cached_byte_size_ = data_size;
    GOOGLE_SAFE_CONCURRENT_WRITES_END();
    total_size += data_size;
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Counters::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Counters* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Counters*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Counters::MergeFrom(const Counters& from) {
  GOOGLE_CHECK_NE(&from, this);
  value_.MergeFrom(from.value_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Counters::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Counters::CopyFrom(const Counters& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Counters::IsInitialized() const {

  return true;
}

void Counters::Swap(Counters* other) {
  if (other != this) {
    value_.Swap(&other->value_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Counters::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Counters_descriptor_;
  metadata.reflection = Counters_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int ProcessorOutput::kModelIdFieldNumber;
const int ProcessorOutput::kTopicsCountFieldNumber;
const int ProcessorOutput::kItemsProcessedFieldNumber;
const int ProcessorOutput::kTopicCountersFieldNumber;
const int ProcessorOutput::kTokenCountersFieldNumber;
const int ProcessorOutput::kTokenFieldNumber;
const int ProcessorOutput::kDiscoveredTokenFieldNumber;
#endif  // !_MSC_VER

ProcessorOutput::ProcessorOutput()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ProcessorOutput::InitAsDefaultInstance() {
  topic_counters_ = const_cast< ::artm::core::Counters*>(&::artm::core::Counters::default_instance());
}

ProcessorOutput::ProcessorOutput(const ProcessorOutput& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ProcessorOutput::SharedCtor() {
  _cached_size_ = 0;
  model_id_ = 0;
  topics_count_ = 0;
  items_processed_ = 0;
  topic_counters_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ProcessorOutput::~ProcessorOutput() {
  SharedDtor();
}

void ProcessorOutput::SharedDtor() {
  if (this != default_instance_) {
    delete topic_counters_;
  }
}

void ProcessorOutput::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ProcessorOutput::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ProcessorOutput_descriptor_;
}

const ProcessorOutput& ProcessorOutput::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_internals_2eproto();
  return *default_instance_;
}

ProcessorOutput* ProcessorOutput::default_instance_ = NULL;

ProcessorOutput* ProcessorOutput::New() const {
  return new ProcessorOutput;
}

void ProcessorOutput::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    model_id_ = 0;
    topics_count_ = 0;
    items_processed_ = 0;
    if (has_topic_counters()) {
      if (topic_counters_ != NULL) topic_counters_->::artm::core::Counters::Clear();
    }
  }
  token_counters_.Clear();
  token_.Clear();
  discovered_token_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ProcessorOutput::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 model_id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &model_id_)));
          set_has_model_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_topics_count;
        break;
      }

      // required int32 topics_count = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_topics_count:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &topics_count_)));
          set_has_topics_count();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_items_processed;
        break;
      }

      // optional int32 items_processed = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_items_processed:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &items_processed_)));
          set_has_items_processed();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_topic_counters;
        break;
      }

      // required .artm.core.Counters topic_counters = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_topic_counters:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_topic_counters()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_token_counters;
        break;
      }

      // repeated .artm.core.Counters token_counters = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_token_counters:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_token_counters()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_token_counters;
        if (input->ExpectTag(50)) goto parse_token;
        break;
      }

      // repeated string token = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_token:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_token()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->token(this->token_size() - 1).data(),
            this->token(this->token_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(50)) goto parse_token;
        if (input->ExpectTag(58)) goto parse_discovered_token;
        break;
      }

      // repeated string discovered_token = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_discovered_token:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_discovered_token()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->discovered_token(this->discovered_token_size() - 1).data(),
            this->discovered_token(this->discovered_token_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(58)) goto parse_discovered_token;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ProcessorOutput::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 model_id = 1;
  if (has_model_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->model_id(), output);
  }

  // required int32 topics_count = 2;
  if (has_topics_count()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->topics_count(), output);
  }

  // optional int32 items_processed = 3;
  if (has_items_processed()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->items_processed(), output);
  }

  // required .artm.core.Counters topic_counters = 4;
  if (has_topic_counters()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      4, this->topic_counters(), output);
  }

  // repeated .artm.core.Counters token_counters = 5;
  for (int i = 0; i < this->token_counters_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      5, this->token_counters(i), output);
  }

  // repeated string token = 6;
  for (int i = 0; i < this->token_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->token(i).data(), this->token(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      6, this->token(i), output);
  }

  // repeated string discovered_token = 7;
  for (int i = 0; i < this->discovered_token_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->discovered_token(i).data(), this->discovered_token(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      7, this->discovered_token(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ProcessorOutput::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 model_id = 1;
  if (has_model_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->model_id(), target);
  }

  // required int32 topics_count = 2;
  if (has_topics_count()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->topics_count(), target);
  }

  // optional int32 items_processed = 3;
  if (has_items_processed()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->items_processed(), target);
  }

  // required .artm.core.Counters topic_counters = 4;
  if (has_topic_counters()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        4, this->topic_counters(), target);
  }

  // repeated .artm.core.Counters token_counters = 5;
  for (int i = 0; i < this->token_counters_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        5, this->token_counters(i), target);
  }

  // repeated string token = 6;
  for (int i = 0; i < this->token_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->token(i).data(), this->token(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(6, this->token(i), target);
  }

  // repeated string discovered_token = 7;
  for (int i = 0; i < this->discovered_token_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->discovered_token(i).data(), this->discovered_token(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(7, this->discovered_token(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ProcessorOutput::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 model_id = 1;
    if (has_model_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->model_id());
    }

    // required int32 topics_count = 2;
    if (has_topics_count()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->topics_count());
    }

    // optional int32 items_processed = 3;
    if (has_items_processed()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->items_processed());
    }

    // required .artm.core.Counters topic_counters = 4;
    if (has_topic_counters()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->topic_counters());
    }

  }
  // repeated .artm.core.Counters token_counters = 5;
  total_size += 1 * this->token_counters_size();
  for (int i = 0; i < this->token_counters_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->token_counters(i));
  }

  // repeated string token = 6;
  total_size += 1 * this->token_size();
  for (int i = 0; i < this->token_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->token(i));
  }

  // repeated string discovered_token = 7;
  total_size += 1 * this->discovered_token_size();
  for (int i = 0; i < this->discovered_token_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->discovered_token(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ProcessorOutput::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ProcessorOutput* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ProcessorOutput*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ProcessorOutput::MergeFrom(const ProcessorOutput& from) {
  GOOGLE_CHECK_NE(&from, this);
  token_counters_.MergeFrom(from.token_counters_);
  token_.MergeFrom(from.token_);
  discovered_token_.MergeFrom(from.discovered_token_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_model_id()) {
      set_model_id(from.model_id());
    }
    if (from.has_topics_count()) {
      set_topics_count(from.topics_count());
    }
    if (from.has_items_processed()) {
      set_items_processed(from.items_processed());
    }
    if (from.has_topic_counters()) {
      mutable_topic_counters()->::artm::core::Counters::MergeFrom(from.topic_counters());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ProcessorOutput::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ProcessorOutput::CopyFrom(const ProcessorOutput& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ProcessorOutput::IsInitialized() const {
  if ((_has_bits_[0] & 0x0000000b) != 0x0000000b) return false;

  return true;
}

void ProcessorOutput::Swap(ProcessorOutput* other) {
  if (other != this) {
    std::swap(model_id_, other->model_id_);
    std::swap(topics_count_, other->topics_count_);
    std::swap(items_processed_, other->items_processed_);
    std::swap(topic_counters_, other->topic_counters_);
    token_counters_.Swap(&other->token_counters_);
    token_.Swap(&other->token_);
    discovered_token_.Swap(&other->discovered_token_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ProcessorOutput::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ProcessorOutput_descriptor_;
  metadata.reflection = ProcessorOutput_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace core
}  // namespace artm

// @@protoc_insertion_point(global_scope)
