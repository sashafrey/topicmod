# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: messages.proto

from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)




DESCRIPTOR = _descriptor.FileDescriptor(
  name='messages.proto',
  package='topicmd',
  serialized_pb='\n\x0emessages.proto\x12\x07topicmd\"I\n\x05\x46ield\x12\x19\n\nfield_name\x18\x01 \x01(\t:\x05@body\x12\x10\n\x08token_id\x18\x02 \x03(\x05\x12\x13\n\x0btoken_count\x18\x03 \x03(\x05\"d\n\x04Item\x12\n\n\x02id\x18\x01 \x01(\x05\x12\x31\n\toperation\x18\x02 \x01(\x0e\x32\x16.topicmd.OperationType:\x06INSERT\x12\x1d\n\x05\x66ield\x18\x03 \x03(\x0b\x32\x0e.topicmd.Field\"3\n\x05\x42\x61tch\x12\r\n\x05token\x18\x01 \x03(\t\x12\x1b\n\x04item\x18\x03 \x03(\x0b\x32\r.topicmd.Item\"\x8e\x01\n\x0eInstanceConfig\x12\x16\n\x0eindex_location\x18\x02 \x01(\t\x12\'\n\x18process_items_on_publish\x18\x03 \x01(\x08:\x05\x66\x61lse\x12\x1e\n\x13index_readers_count\x18\x04 \x01(\x05:\x01\x31\x12\x1b\n\x10processors_count\x18\x05 \x01(\x05:\x01\x31\"\xab\x01\n\x0bModelConfig\x12\x46\n\x0eprocessor_type\x18\x01 \x01(\x0e\x32\".topicmd.ModelConfig.ProcessorType:\nBATCH_PLSA\x12\x18\n\x0ctopics_count\x18\x02 \x01(\x05:\x02\x33\x32\x12\x19\n\nis_enabled\x18\x03 \x01(\x08:\x05\x66\x61lse\"\x1f\n\rProcessorType\x12\x0e\n\nBATCH_PLSA\x10\x00\"\x9c\x01\n\x0cLoggerConfig\x12\x14\n\x0clog_location\x18\x01 \x01(\t\x12\x30\n\x05level\x18\x02 \x01(\x0e\x32\x1b.topicmd.LoggerConfig.Level:\x04INFO\"D\n\x05Level\x12\x0c\n\x08\x44ISABLED\x10\x00\x12\t\n\x05\x45RROR\x10\x01\x12\x0b\n\x07WARNING\x10\x02\x12\x08\n\x04INFO\x10\x03\x12\x0b\n\x07VERBOSE\x10\x04\".\n\nItemTopics\x12\n\n\x02id\x18\x01 \x01(\x05\x12\x14\n\x0ctopic_weight\x18\x02 \x03(\x02\"7\n\x0b\x42\x61tchTopics\x12(\n\x0bitem_topics\x18\x01 \x03(\x0b\x32\x13.topicmd.ItemTopics\"D\n\x0bTokenTopics\x12\r\n\x05token\x18\x01 \x01(\t\x12\x10\n\x08token_id\x18\x02 \x01(\x05\x12\x14\n\x0ctopic_weight\x18\x03 \x03(\x02\"8\n\x0bModelTopics\x12)\n\x0btoken_topic\x18\x01 \x03(\x0b\x32\x14.topicmd.TokenTopics*\x1b\n\rOperationType\x12\n\n\x06INSERT\x10\x00')

_OPERATIONTYPE = _descriptor.EnumDescriptor(
  name='OperationType',
  full_name='topicmd.OperationType',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='INSERT', index=0, number=0,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=968,
  serialized_end=995,
)

OperationType = enum_type_wrapper.EnumTypeWrapper(_OPERATIONTYPE)
INSERT = 0


_MODELCONFIG_PROCESSORTYPE = _descriptor.EnumDescriptor(
  name='ProcessorType',
  full_name='topicmd.ModelConfig.ProcessorType',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='BATCH_PLSA', index=0, number=0,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=543,
  serialized_end=574,
)

_LOGGERCONFIG_LEVEL = _descriptor.EnumDescriptor(
  name='Level',
  full_name='topicmd.LoggerConfig.Level',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='DISABLED', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ERROR', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='WARNING', index=2, number=2,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='INFO', index=3, number=3,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='VERBOSE', index=4, number=4,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=665,
  serialized_end=733,
)


_FIELD = _descriptor.Descriptor(
  name='Field',
  full_name='topicmd.Field',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='field_name', full_name='topicmd.Field.field_name', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=unicode("@body", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token_id', full_name='topicmd.Field.token_id', index=1,
      number=2, type=5, cpp_type=1, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token_count', full_name='topicmd.Field.token_count', index=2,
      number=3, type=5, cpp_type=1, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=27,
  serialized_end=100,
)


_ITEM = _descriptor.Descriptor(
  name='Item',
  full_name='topicmd.Item',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='topicmd.Item.id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='operation', full_name='topicmd.Item.operation', index=1,
      number=2, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='field', full_name='topicmd.Item.field', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=102,
  serialized_end=202,
)


_BATCH = _descriptor.Descriptor(
  name='Batch',
  full_name='topicmd.Batch',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='token', full_name='topicmd.Batch.token', index=0,
      number=1, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='item', full_name='topicmd.Batch.item', index=1,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=204,
  serialized_end=255,
)


_INSTANCECONFIG = _descriptor.Descriptor(
  name='InstanceConfig',
  full_name='topicmd.InstanceConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='index_location', full_name='topicmd.InstanceConfig.index_location', index=0,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='process_items_on_publish', full_name='topicmd.InstanceConfig.process_items_on_publish', index=1,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='index_readers_count', full_name='topicmd.InstanceConfig.index_readers_count', index=2,
      number=4, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='processors_count', full_name='topicmd.InstanceConfig.processors_count', index=3,
      number=5, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=258,
  serialized_end=400,
)


_MODELCONFIG = _descriptor.Descriptor(
  name='ModelConfig',
  full_name='topicmd.ModelConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='processor_type', full_name='topicmd.ModelConfig.processor_type', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='topics_count', full_name='topicmd.ModelConfig.topics_count', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=32,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='is_enabled', full_name='topicmd.ModelConfig.is_enabled', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _MODELCONFIG_PROCESSORTYPE,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=403,
  serialized_end=574,
)


_LOGGERCONFIG = _descriptor.Descriptor(
  name='LoggerConfig',
  full_name='topicmd.LoggerConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='log_location', full_name='topicmd.LoggerConfig.log_location', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='level', full_name='topicmd.LoggerConfig.level', index=1,
      number=2, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=3,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _LOGGERCONFIG_LEVEL,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=577,
  serialized_end=733,
)


_ITEMTOPICS = _descriptor.Descriptor(
  name='ItemTopics',
  full_name='topicmd.ItemTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='topicmd.ItemTopics.id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='topic_weight', full_name='topicmd.ItemTopics.topic_weight', index=1,
      number=2, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=735,
  serialized_end=781,
)


_BATCHTOPICS = _descriptor.Descriptor(
  name='BatchTopics',
  full_name='topicmd.BatchTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='item_topics', full_name='topicmd.BatchTopics.item_topics', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=783,
  serialized_end=838,
)


_TOKENTOPICS = _descriptor.Descriptor(
  name='TokenTopics',
  full_name='topicmd.TokenTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='token', full_name='topicmd.TokenTopics.token', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token_id', full_name='topicmd.TokenTopics.token_id', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='topic_weight', full_name='topicmd.TokenTopics.topic_weight', index=2,
      number=3, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=840,
  serialized_end=908,
)


_MODELTOPICS = _descriptor.Descriptor(
  name='ModelTopics',
  full_name='topicmd.ModelTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='token_topic', full_name='topicmd.ModelTopics.token_topic', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=910,
  serialized_end=966,
)

_ITEM.fields_by_name['operation'].enum_type = _OPERATIONTYPE
_ITEM.fields_by_name['field'].message_type = _FIELD
_BATCH.fields_by_name['item'].message_type = _ITEM
_MODELCONFIG.fields_by_name['processor_type'].enum_type = _MODELCONFIG_PROCESSORTYPE
_MODELCONFIG_PROCESSORTYPE.containing_type = _MODELCONFIG;
_LOGGERCONFIG.fields_by_name['level'].enum_type = _LOGGERCONFIG_LEVEL
_LOGGERCONFIG_LEVEL.containing_type = _LOGGERCONFIG;
_BATCHTOPICS.fields_by_name['item_topics'].message_type = _ITEMTOPICS
_MODELTOPICS.fields_by_name['token_topic'].message_type = _TOKENTOPICS
DESCRIPTOR.message_types_by_name['Field'] = _FIELD
DESCRIPTOR.message_types_by_name['Item'] = _ITEM
DESCRIPTOR.message_types_by_name['Batch'] = _BATCH
DESCRIPTOR.message_types_by_name['InstanceConfig'] = _INSTANCECONFIG
DESCRIPTOR.message_types_by_name['ModelConfig'] = _MODELCONFIG
DESCRIPTOR.message_types_by_name['LoggerConfig'] = _LOGGERCONFIG
DESCRIPTOR.message_types_by_name['ItemTopics'] = _ITEMTOPICS
DESCRIPTOR.message_types_by_name['BatchTopics'] = _BATCHTOPICS
DESCRIPTOR.message_types_by_name['TokenTopics'] = _TOKENTOPICS
DESCRIPTOR.message_types_by_name['ModelTopics'] = _MODELTOPICS

class Field(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _FIELD

  # @@protoc_insertion_point(class_scope:topicmd.Field)

class Item(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ITEM

  # @@protoc_insertion_point(class_scope:topicmd.Item)

class Batch(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BATCH

  # @@protoc_insertion_point(class_scope:topicmd.Batch)

class InstanceConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _INSTANCECONFIG

  # @@protoc_insertion_point(class_scope:topicmd.InstanceConfig)

class ModelConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MODELCONFIG

  # @@protoc_insertion_point(class_scope:topicmd.ModelConfig)

class LoggerConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _LOGGERCONFIG

  # @@protoc_insertion_point(class_scope:topicmd.LoggerConfig)

class ItemTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ITEMTOPICS

  # @@protoc_insertion_point(class_scope:topicmd.ItemTopics)

class BatchTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BATCHTOPICS

  # @@protoc_insertion_point(class_scope:topicmd.BatchTopics)

class TokenTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _TOKENTOPICS

  # @@protoc_insertion_point(class_scope:topicmd.TokenTopics)

class ModelTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MODELTOPICS

  # @@protoc_insertion_point(class_scope:topicmd.ModelTopics)


# @@protoc_insertion_point(module_scope)
