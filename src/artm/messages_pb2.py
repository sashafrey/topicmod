# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: messages.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)




DESCRIPTOR = _descriptor.FileDescriptor(
  name='messages.proto',
  package='artm',
  serialized_pb='\n\x0emessages.proto\x12\x04\x61rtm\".\n\x04Item\x12\n\n\x02id\x18\x01 \x01(\x05\x12\x1a\n\x05\x66ield\x18\x02 \x03(\x0b\x32\x0b.artm.Field\"I\n\x05\x46ield\x12\x19\n\nfield_name\x18\x01 \x01(\t:\x05@body\x12\x10\n\x08token_id\x18\x02 \x03(\x05\x12\x13\n\x0btoken_count\x18\x03 \x03(\x05\"0\n\x05\x42\x61tch\x12\r\n\x05token\x18\x01 \x03(\t\x12\x18\n\x04item\x18\x02 \x03(\x0b\x32\n.artm.Item\"p\n\x10\x44\x61taLoaderConfig\x12\x13\n\x0binstance_id\x18\x01 \x02(\x05\x12\x11\n\tdisk_path\x18\x02 \x01(\t\x12\x16\n\nqueue_size\x18\x03 \x01(\x05:\x02\x31\x30\x12\x1c\n\x06stream\x18\x04 \x03(\x0b\x32\x0c.artm.Stream\"\xa8\x01\n\x06Stream\x12\'\n\x04type\x18\x01 \x02(\x0e\x32\x11.artm.Stream.Type:\x06Global\x12\x15\n\x04name\x18\x02 \x01(\t:\x07@global\x12\x0f\n\x07modulus\x18\x03 \x01(\x05\x12\x11\n\tresiduals\x18\x04 \x03(\x05\":\n\x04Type\x12\n\n\x06Global\x10\x00\x12\x11\n\rItemIdModulus\x10\x01\x12\x13\n\x0fItemHashModulus\x10\x03\"I\n\x0eInstanceConfig\x12\x1b\n\x10processors_count\x18\x01 \x01(\x05:\x01\x31\x12\x1a\n\x12memcached_endpoint\x18\x02 \x01(\t\"\x93\x02\n\x0bModelConfig\x12\x18\n\x0ctopics_count\x18\x02 \x01(\x05:\x02\x33\x32\x12\x16\n\x07\x65nabled\x18\x03 \x01(\x08:\x05\x66\x61lse\x12\"\n\x16inner_iterations_count\x18\x04 \x01(\x05:\x02\x31\x30\x12\x19\n\nfield_name\x18\x05 \x01(\t:\x05@body\x12\x1c\n\x0bstream_name\x18\x06 \x01(\t:\x07@global\x12\x1a\n\x05score\x18\x07 \x03(\x0b\x32\x0b.artm.Score\x12\x1a\n\x0breuse_theta\x18\x08 \x01(\x08:\x05\x66\x61lse\x12#\n\x14\x63\x61\x63he_token_counters\x18\t \x01(\x08:\x05\x66\x61lse\x12\x18\n\x10regularizer_name\x18\n \x03(\t\"\xb7\x01\n\x11RegularizerConfig\x12\x0c\n\x04name\x18\x01 \x02(\t\x12*\n\x04type\x18\x02 \x02(\x0e\x32\x1c.artm.RegularizerConfig.Type\x12\x0e\n\x06\x63onfig\x18\x03 \x02(\x0c\"X\n\x04Type\x12\x12\n\x0e\x44irichletTheta\x10\x00\x12\x10\n\x0c\x44irichletPhi\x10\x01\x12\x15\n\x11SmoothSparseTheta\x10\x02\x12\x13\n\x0fSmoothSparsePhi\x10\x03\"O\n\x14\x44irichletThetaConfig\x12\x0f\n\x07\x61lpha_0\x18\x01 \x03(\x01\x12&\n\x0btilde_alpha\x18\x02 \x03(\x0b\x32\x11.artm.DoubleArray\"K\n\x12\x44irichletPhiConfig\x12\x0e\n\x06\x62\x65ta_0\x18\x01 \x02(\x01\x12%\n\ntilde_beta\x18\x02 \x02(\x0b\x32\x11.artm.DoubleArray\"\xc2\x01\n\x17SmoothSparseThetaConfig\x12\x1f\n\x17\x62\x61\x63kground_topics_count\x18\x01 \x02(\x05\x12\x0f\n\x07\x61lpha_0\x18\x02 \x03(\x01\x12&\n\x0btilde_alpha\x18\x03 \x03(\x0b\x32\x11.artm.DoubleArray\x12\x1a\n\x12\x62\x61\x63kground_alpha_0\x18\x04 \x03(\x01\x12\x31\n\x16\x62\x61\x63kground_tilde_alpha\x18\x05 \x03(\x0b\x32\x11.artm.DoubleArray\"\xbc\x01\n\x15SmoothSparsePhiConfig\x12\x1f\n\x17\x62\x61\x63kground_topics_count\x18\x01 \x02(\x05\x12\x0e\n\x06\x62\x65ta_0\x18\x02 \x02(\x01\x12%\n\ntilde_beta\x18\x03 \x02(\x0b\x32\x11.artm.DoubleArray\x12\x19\n\x11\x62\x61\x63kground_beta_0\x18\x04 \x03(\x01\x12\x30\n\x15\x62\x61\x63kground_tilde_beta\x18\x05 \x03(\x0b\x32\x11.artm.DoubleArray\"\x1c\n\x0b\x44oubleArray\x12\r\n\x05value\x18\x01 \x03(\x01\"x\n\x05Score\x12\x1e\n\x04type\x18\x01 \x02(\x0e\x32\x10.artm.Score.Type\x12\x19\n\nfield_name\x18\x02 \x01(\t:\x05@body\x12\x1c\n\x0bstream_name\x18\x03 \x01(\t:\x07@global\"\x16\n\x04Type\x12\x0e\n\nPerplexity\x10\x00\"\x99\x01\n\x0cLoggerConfig\x12\x14\n\x0clog_location\x18\x01 \x01(\t\x12-\n\x05level\x18\x02 \x01(\x0e\x32\x18.artm.LoggerConfig.Level:\x04INFO\"D\n\x05Level\x12\x0c\n\x08\x44ISABLED\x10\x00\x12\t\n\x05\x45RROR\x10\x01\x12\x0b\n\x07WARNING\x10\x02\x12\x08\n\x04INFO\x10\x03\x12\x0b\n\x07VERBOSE\x10\x04\"]\n\x0bModelTopics\x12&\n\x0btoken_topic\x18\x01 \x03(\x0b\x32\x11.artm.TokenTopics\x12\x17\n\x0fitems_processed\x18\x02 \x01(\x05\x12\r\n\x05score\x18\x03 \x03(\x01\"D\n\x0bTokenTopics\x12\r\n\x05token\x18\x01 \x01(\t\x12\x10\n\x08token_id\x18\x02 \x01(\x05\x12\x14\n\x0ctopic_weight\x18\x03 \x03(\x02\".\n\nItemTopics\x12\n\n\x02id\x18\x01 \x01(\x05\x12\x14\n\x0ctopic_weight\x18\x02 \x03(\x02\"4\n\x0b\x42\x61tchTopics\x12%\n\x0bitem_topics\x18\x01 \x03(\x0b\x32\x10.artm.ItemTopics')



_STREAM_TYPE = _descriptor.EnumDescriptor(
  name='Type',
  full_name='artm.Stream.Type',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='Global', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ItemIdModulus', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ItemHashModulus', index=2, number=3,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=422,
  serialized_end=480,
)

_REGULARIZERCONFIG_TYPE = _descriptor.EnumDescriptor(
  name='Type',
  full_name='artm.RegularizerConfig.Type',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='DirichletTheta', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='DirichletPhi', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='SmoothSparseTheta', index=2, number=2,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='SmoothSparsePhi', index=3, number=3,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=931,
  serialized_end=1019,
)

_SCORE_TYPE = _descriptor.EnumDescriptor(
  name='Type',
  full_name='artm.Score.Type',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='Perplexity', index=0, number=0,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=1695,
  serialized_end=1717,
)

_LOGGERCONFIG_LEVEL = _descriptor.EnumDescriptor(
  name='Level',
  full_name='artm.LoggerConfig.Level',
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
  serialized_start=1805,
  serialized_end=1873,
)


_ITEM = _descriptor.Descriptor(
  name='Item',
  full_name='artm.Item',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='artm.Item.id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='field', full_name='artm.Item.field', index=1,
      number=2, type=11, cpp_type=10, label=3,
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
  serialized_start=24,
  serialized_end=70,
)


_FIELD = _descriptor.Descriptor(
  name='Field',
  full_name='artm.Field',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='field_name', full_name='artm.Field.field_name', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=unicode("@body", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token_id', full_name='artm.Field.token_id', index=1,
      number=2, type=5, cpp_type=1, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token_count', full_name='artm.Field.token_count', index=2,
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
  serialized_start=72,
  serialized_end=145,
)


_BATCH = _descriptor.Descriptor(
  name='Batch',
  full_name='artm.Batch',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='token', full_name='artm.Batch.token', index=0,
      number=1, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='item', full_name='artm.Batch.item', index=1,
      number=2, type=11, cpp_type=10, label=3,
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
  serialized_start=147,
  serialized_end=195,
)


_DATALOADERCONFIG = _descriptor.Descriptor(
  name='DataLoaderConfig',
  full_name='artm.DataLoaderConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='instance_id', full_name='artm.DataLoaderConfig.instance_id', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='disk_path', full_name='artm.DataLoaderConfig.disk_path', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='queue_size', full_name='artm.DataLoaderConfig.queue_size', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=10,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='stream', full_name='artm.DataLoaderConfig.stream', index=3,
      number=4, type=11, cpp_type=10, label=3,
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
  serialized_start=197,
  serialized_end=309,
)


_STREAM = _descriptor.Descriptor(
  name='Stream',
  full_name='artm.Stream',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='type', full_name='artm.Stream.type', index=0,
      number=1, type=14, cpp_type=8, label=2,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='name', full_name='artm.Stream.name', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=unicode("@global", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='modulus', full_name='artm.Stream.modulus', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='residuals', full_name='artm.Stream.residuals', index=3,
      number=4, type=5, cpp_type=1, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _STREAM_TYPE,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=312,
  serialized_end=480,
)


_INSTANCECONFIG = _descriptor.Descriptor(
  name='InstanceConfig',
  full_name='artm.InstanceConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='processors_count', full_name='artm.InstanceConfig.processors_count', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='memcached_endpoint', full_name='artm.InstanceConfig.memcached_endpoint', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
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
  serialized_start=482,
  serialized_end=555,
)


_MODELCONFIG = _descriptor.Descriptor(
  name='ModelConfig',
  full_name='artm.ModelConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='topics_count', full_name='artm.ModelConfig.topics_count', index=0,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=32,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='enabled', full_name='artm.ModelConfig.enabled', index=1,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='inner_iterations_count', full_name='artm.ModelConfig.inner_iterations_count', index=2,
      number=4, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=10,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='field_name', full_name='artm.ModelConfig.field_name', index=3,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=unicode("@body", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='stream_name', full_name='artm.ModelConfig.stream_name', index=4,
      number=6, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=unicode("@global", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='score', full_name='artm.ModelConfig.score', index=5,
      number=7, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='reuse_theta', full_name='artm.ModelConfig.reuse_theta', index=6,
      number=8, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cache_token_counters', full_name='artm.ModelConfig.cache_token_counters', index=7,
      number=9, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='regularizer_name', full_name='artm.ModelConfig.regularizer_name', index=8,
      number=10, type=9, cpp_type=9, label=3,
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
  serialized_start=558,
  serialized_end=833,
)


_REGULARIZERCONFIG = _descriptor.Descriptor(
  name='RegularizerConfig',
  full_name='artm.RegularizerConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='artm.RegularizerConfig.name', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='type', full_name='artm.RegularizerConfig.type', index=1,
      number=2, type=14, cpp_type=8, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='config', full_name='artm.RegularizerConfig.config', index=2,
      number=3, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _REGULARIZERCONFIG_TYPE,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=836,
  serialized_end=1019,
)


_DIRICHLETTHETACONFIG = _descriptor.Descriptor(
  name='DirichletThetaConfig',
  full_name='artm.DirichletThetaConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='alpha_0', full_name='artm.DirichletThetaConfig.alpha_0', index=0,
      number=1, type=1, cpp_type=5, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='tilde_alpha', full_name='artm.DirichletThetaConfig.tilde_alpha', index=1,
      number=2, type=11, cpp_type=10, label=3,
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
  serialized_start=1021,
  serialized_end=1100,
)


_DIRICHLETPHICONFIG = _descriptor.Descriptor(
  name='DirichletPhiConfig',
  full_name='artm.DirichletPhiConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='beta_0', full_name='artm.DirichletPhiConfig.beta_0', index=0,
      number=1, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='tilde_beta', full_name='artm.DirichletPhiConfig.tilde_beta', index=1,
      number=2, type=11, cpp_type=10, label=2,
      has_default_value=False, default_value=None,
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
  serialized_start=1102,
  serialized_end=1177,
)


_SMOOTHSPARSETHETACONFIG = _descriptor.Descriptor(
  name='SmoothSparseThetaConfig',
  full_name='artm.SmoothSparseThetaConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='background_topics_count', full_name='artm.SmoothSparseThetaConfig.background_topics_count', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='alpha_0', full_name='artm.SmoothSparseThetaConfig.alpha_0', index=1,
      number=2, type=1, cpp_type=5, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='tilde_alpha', full_name='artm.SmoothSparseThetaConfig.tilde_alpha', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='background_alpha_0', full_name='artm.SmoothSparseThetaConfig.background_alpha_0', index=3,
      number=4, type=1, cpp_type=5, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='background_tilde_alpha', full_name='artm.SmoothSparseThetaConfig.background_tilde_alpha', index=4,
      number=5, type=11, cpp_type=10, label=3,
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
  serialized_start=1180,
  serialized_end=1374,
)


_SMOOTHSPARSEPHICONFIG = _descriptor.Descriptor(
  name='SmoothSparsePhiConfig',
  full_name='artm.SmoothSparsePhiConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='background_topics_count', full_name='artm.SmoothSparsePhiConfig.background_topics_count', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='beta_0', full_name='artm.SmoothSparsePhiConfig.beta_0', index=1,
      number=2, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='tilde_beta', full_name='artm.SmoothSparsePhiConfig.tilde_beta', index=2,
      number=3, type=11, cpp_type=10, label=2,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='background_beta_0', full_name='artm.SmoothSparsePhiConfig.background_beta_0', index=3,
      number=4, type=1, cpp_type=5, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='background_tilde_beta', full_name='artm.SmoothSparsePhiConfig.background_tilde_beta', index=4,
      number=5, type=11, cpp_type=10, label=3,
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
  serialized_start=1377,
  serialized_end=1565,
)


_DOUBLEARRAY = _descriptor.Descriptor(
  name='DoubleArray',
  full_name='artm.DoubleArray',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='value', full_name='artm.DoubleArray.value', index=0,
      number=1, type=1, cpp_type=5, label=3,
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
  serialized_start=1567,
  serialized_end=1595,
)


_SCORE = _descriptor.Descriptor(
  name='Score',
  full_name='artm.Score',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='type', full_name='artm.Score.type', index=0,
      number=1, type=14, cpp_type=8, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='field_name', full_name='artm.Score.field_name', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=unicode("@body", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='stream_name', full_name='artm.Score.stream_name', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=True, default_value=unicode("@global", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _SCORE_TYPE,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1597,
  serialized_end=1717,
)


_LOGGERCONFIG = _descriptor.Descriptor(
  name='LoggerConfig',
  full_name='artm.LoggerConfig',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='log_location', full_name='artm.LoggerConfig.log_location', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='level', full_name='artm.LoggerConfig.level', index=1,
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
  serialized_start=1720,
  serialized_end=1873,
)


_MODELTOPICS = _descriptor.Descriptor(
  name='ModelTopics',
  full_name='artm.ModelTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='token_topic', full_name='artm.ModelTopics.token_topic', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='items_processed', full_name='artm.ModelTopics.items_processed', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='score', full_name='artm.ModelTopics.score', index=2,
      number=3, type=1, cpp_type=5, label=3,
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
  serialized_start=1875,
  serialized_end=1968,
)


_TOKENTOPICS = _descriptor.Descriptor(
  name='TokenTopics',
  full_name='artm.TokenTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='token', full_name='artm.TokenTopics.token', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token_id', full_name='artm.TokenTopics.token_id', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='topic_weight', full_name='artm.TokenTopics.topic_weight', index=2,
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
  serialized_start=1970,
  serialized_end=2038,
)


_ITEMTOPICS = _descriptor.Descriptor(
  name='ItemTopics',
  full_name='artm.ItemTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='artm.ItemTopics.id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='topic_weight', full_name='artm.ItemTopics.topic_weight', index=1,
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
  serialized_start=2040,
  serialized_end=2086,
)


_BATCHTOPICS = _descriptor.Descriptor(
  name='BatchTopics',
  full_name='artm.BatchTopics',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='item_topics', full_name='artm.BatchTopics.item_topics', index=0,
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
  serialized_start=2088,
  serialized_end=2140,
)

_ITEM.fields_by_name['field'].message_type = _FIELD
_BATCH.fields_by_name['item'].message_type = _ITEM
_DATALOADERCONFIG.fields_by_name['stream'].message_type = _STREAM
_STREAM.fields_by_name['type'].enum_type = _STREAM_TYPE
_STREAM_TYPE.containing_type = _STREAM;
_MODELCONFIG.fields_by_name['score'].message_type = _SCORE
_REGULARIZERCONFIG.fields_by_name['type'].enum_type = _REGULARIZERCONFIG_TYPE
_REGULARIZERCONFIG_TYPE.containing_type = _REGULARIZERCONFIG;
_DIRICHLETTHETACONFIG.fields_by_name['tilde_alpha'].message_type = _DOUBLEARRAY
_DIRICHLETPHICONFIG.fields_by_name['tilde_beta'].message_type = _DOUBLEARRAY
_SMOOTHSPARSETHETACONFIG.fields_by_name['tilde_alpha'].message_type = _DOUBLEARRAY
_SMOOTHSPARSETHETACONFIG.fields_by_name['background_tilde_alpha'].message_type = _DOUBLEARRAY
_SMOOTHSPARSEPHICONFIG.fields_by_name['tilde_beta'].message_type = _DOUBLEARRAY
_SMOOTHSPARSEPHICONFIG.fields_by_name['background_tilde_beta'].message_type = _DOUBLEARRAY
_SCORE.fields_by_name['type'].enum_type = _SCORE_TYPE
_SCORE_TYPE.containing_type = _SCORE;
_LOGGERCONFIG.fields_by_name['level'].enum_type = _LOGGERCONFIG_LEVEL
_LOGGERCONFIG_LEVEL.containing_type = _LOGGERCONFIG;
_MODELTOPICS.fields_by_name['token_topic'].message_type = _TOKENTOPICS
_BATCHTOPICS.fields_by_name['item_topics'].message_type = _ITEMTOPICS
DESCRIPTOR.message_types_by_name['Item'] = _ITEM
DESCRIPTOR.message_types_by_name['Field'] = _FIELD
DESCRIPTOR.message_types_by_name['Batch'] = _BATCH
DESCRIPTOR.message_types_by_name['DataLoaderConfig'] = _DATALOADERCONFIG
DESCRIPTOR.message_types_by_name['Stream'] = _STREAM
DESCRIPTOR.message_types_by_name['InstanceConfig'] = _INSTANCECONFIG
DESCRIPTOR.message_types_by_name['ModelConfig'] = _MODELCONFIG
DESCRIPTOR.message_types_by_name['RegularizerConfig'] = _REGULARIZERCONFIG
DESCRIPTOR.message_types_by_name['DirichletThetaConfig'] = _DIRICHLETTHETACONFIG
DESCRIPTOR.message_types_by_name['DirichletPhiConfig'] = _DIRICHLETPHICONFIG
DESCRIPTOR.message_types_by_name['SmoothSparseThetaConfig'] = _SMOOTHSPARSETHETACONFIG
DESCRIPTOR.message_types_by_name['SmoothSparsePhiConfig'] = _SMOOTHSPARSEPHICONFIG
DESCRIPTOR.message_types_by_name['DoubleArray'] = _DOUBLEARRAY
DESCRIPTOR.message_types_by_name['Score'] = _SCORE
DESCRIPTOR.message_types_by_name['LoggerConfig'] = _LOGGERCONFIG
DESCRIPTOR.message_types_by_name['ModelTopics'] = _MODELTOPICS
DESCRIPTOR.message_types_by_name['TokenTopics'] = _TOKENTOPICS
DESCRIPTOR.message_types_by_name['ItemTopics'] = _ITEMTOPICS
DESCRIPTOR.message_types_by_name['BatchTopics'] = _BATCHTOPICS

class Item(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ITEM

  # @@protoc_insertion_point(class_scope:artm.Item)

class Field(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _FIELD

  # @@protoc_insertion_point(class_scope:artm.Field)

class Batch(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BATCH

  # @@protoc_insertion_point(class_scope:artm.Batch)

class DataLoaderConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _DATALOADERCONFIG

  # @@protoc_insertion_point(class_scope:artm.DataLoaderConfig)

class Stream(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _STREAM

  # @@protoc_insertion_point(class_scope:artm.Stream)

class InstanceConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _INSTANCECONFIG

  # @@protoc_insertion_point(class_scope:artm.InstanceConfig)

class ModelConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MODELCONFIG

  # @@protoc_insertion_point(class_scope:artm.ModelConfig)

class RegularizerConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _REGULARIZERCONFIG

  # @@protoc_insertion_point(class_scope:artm.RegularizerConfig)

class DirichletThetaConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _DIRICHLETTHETACONFIG

  # @@protoc_insertion_point(class_scope:artm.DirichletThetaConfig)

class DirichletPhiConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _DIRICHLETPHICONFIG

  # @@protoc_insertion_point(class_scope:artm.DirichletPhiConfig)

class SmoothSparseThetaConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SMOOTHSPARSETHETACONFIG

  # @@protoc_insertion_point(class_scope:artm.SmoothSparseThetaConfig)

class SmoothSparsePhiConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SMOOTHSPARSEPHICONFIG

  # @@protoc_insertion_point(class_scope:artm.SmoothSparsePhiConfig)

class DoubleArray(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _DOUBLEARRAY

  # @@protoc_insertion_point(class_scope:artm.DoubleArray)

class Score(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SCORE

  # @@protoc_insertion_point(class_scope:artm.Score)

class LoggerConfig(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _LOGGERCONFIG

  # @@protoc_insertion_point(class_scope:artm.LoggerConfig)

class ModelTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MODELTOPICS

  # @@protoc_insertion_point(class_scope:artm.ModelTopics)

class TokenTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _TOKENTOPICS

  # @@protoc_insertion_point(class_scope:artm.TokenTopics)

class ItemTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ITEMTOPICS

  # @@protoc_insertion_point(class_scope:artm.ItemTopics)

class BatchTopics(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _BATCHTOPICS

  # @@protoc_insertion_point(class_scope:artm.BatchTopics)


# @@protoc_insertion_point(module_scope)
