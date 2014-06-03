# Copyright 2014, Additive Regularization of Topic Models.

# Author: Murat Apishev (great-mel@yandex.ru)

import os
import sys

if sys.platform.count('linux') == 1:
  interface_address = os.path.abspath(os.path.join(os.curdir, os.pardir, 'python_interface'))
  sys.path.append(interface_address)

import messages_pb2
import python_interface
from python_interface import *

#################################################################################
# ALL CODE BELOW DEFINES PROTOBUF MESSAGES NEED TO TEST THE INTERFACE FUNCTIONS

# Create master_config
master_config = messages_pb2.MasterComponentConfig()
master_config.processors_count = 2
master_config.processor_queue_max_size = 5
stream_ = master_config.stream.add()
stream_.name = ('stream_0')
stream_.type = 0
stream_.modulus = 3
stream_.residuals.append(9)

# Create batch
batch = messages_pb2.Batch()
batch.token.append('first')
item_ = batch.item.add()
item_.id = 2
field_ = item_.field.add()
field_.token_id.append(0)
field_.token_count.append(2)

# Create stream
stream = messages_pb2.Stream()
stream.name = ('stream_8')
stream.type = 0
stream.modulus = 3
stream.residuals.append(1)

# Create regularizer_config
dirichlet_regularizer_config = messages_pb2.DirichletThetaConfig()
tilde_alpha = dirichlet_regularizer_config.tilde_alpha.add()
tilde_alpha.value.append(0.1)

regularizer_config = messages_pb2.RegularizerConfig()
regularizer_config.name = 'regularizer_1'
regularizer_config.type = 0
regularizer_config.config = dirichlet_regularizer_config.SerializeToString()

# Create model_config
model_config = messages_pb2.ModelConfig()
model_config.stream_name = ('stream_0')
score_ = model_config.score.add()
score_.type = 0
score_.stream_name = ('stream_0')
model_config.regularizer_name.append(regularizer_config.name)

# New configs to reconfigure stuff
master_config_new = messages_pb2.MasterComponentConfig()
master_config_new.CopyFrom(master_config);
master_config_new.processors_count = 1
master_config_new.processor_queue_max_size = 2

model_config_new = messages_pb2.ModelConfig()
model_config_new.CopyFrom(model_config)
model_config_new.inner_iterations_count = 20

dirichlet_regularizer_config_new = messages_pb2.DirichletThetaConfig()
tilde_alpha = dirichlet_regularizer_config_new.tilde_alpha.add()
tilde_alpha.value.append(0.2)
regularizer_config_new = messages_pb2.RegularizerConfig()
regularizer_config_new.CopyFrom(regularizer_config)
regularizer_config_new.config = dirichlet_regularizer_config_new.SerializeToString()

#################################################################################
# TEST SECTION

import sys

#if sys.platform.count('linux') == 1:
#  interface_address

address = os.path.abspath(os.path.join(os.curdir, os.pardir))
if sys.platform.count('linux') == 1:
  library = ArtmLibrary(address + '/bin/libartm.so')
else:
  os.environ['PATH'] = ';'.join([address + '\\Win32\\Debug', os.environ['PATH']])
  library = ArtmLibrary(address + '\\Win32\\Debug\\artm.dll')

with library.CreateMasterComponent(master_config) as master_component:
  master_component.AddStream(stream)
  master_component.RemoveStream(stream)
  model = master_component.CreateModel(model_config)
  master_component.RemoveModel(model)
  model = master_component.CreateModel(model_config)

  regularizer = master_component.CreateRegularizer(regularizer_config)
  master_component.RemoveRegularizer(regularizer)
  regularizer = master_component.CreateRegularizer(regularizer_config)

  master_component.AddBatch(batch)
  model.Enable()
  master_component.InvokeIteration(10)
  model.Disable()
  topic_model = master_component.GetTopicModel(model)

  # Test all 'reconfigure' methods
  regularizer.Reconfigure(regularizer_config_new)
  model.Reconfigure(model_config_new)
  master_component.Reconfigure(master_config_new)

print 'All tests have been successfully passed!'
