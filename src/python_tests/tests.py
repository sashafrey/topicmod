# Copyright 2014, Additive Regularization of Topic Models.

import os
import sys

if sys.platform.count('linux') == 1:
  interface_address = os.path.abspath(os.path.join(os.curdir, os.pardir, 'python_interface'))
  sys.path.append(interface_address)
else:
  sys.path.append('../python_interface/')

import messages_pb2
import python_interface
from python_interface import *

#################################################################################
# ALL CODE BELOW DEFINES PROTOBUF MESSAGES NEED TO TEST THE INTERFACE FUNCTIONS

# Create master_config
master_config = messages_pb2.MasterComponentConfig()
master_config.processors_count = 2
master_config.processor_queue_max_size = 5
master_config.cache_processor_output = 1
stream_ = master_config.stream.add()
stream_.name = ('stream_0')
stream_.type = Stream_Type_Global
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
stream.type = Stream_Type_Global
stream.modulus = 3
stream.residuals.append(1)

# Create regularizer_config
dirichlet_regularizer_config = messages_pb2.DirichletThetaConfig()
alpha = dirichlet_regularizer_config.alpha.add()
alpha.value.append(0.1)
general_dirichlet_config = messages_pb2.RegularizerConfig()
general_dirichlet_config.name = 'regularizer_1'
general_dirichlet_config.config = dirichlet_regularizer_config.SerializeToString()
general_dirichlet_config.type = 0

# Create model_config
model_config = messages_pb2.ModelConfig()
model_config.stream_name = ('stream_0')
score_ = model_config.score.add()
score_.type = Score_Type_Perplexity
score_.stream_name = ('stream_0')
model_config.regularizer_name.append('regularizer_1')
model_config.regularizer_tau.append(1)
model_config.regularizer_name.append('regularizer_2')
model_config.regularizer_tau.append(2)

# New configs to reconfigure stuff
master_config_new = messages_pb2.MasterComponentConfig()
master_config_new.CopyFrom(master_config);
master_config_new.processors_count = 1
master_config_new.processor_queue_max_size = 2

model_config_new = messages_pb2.ModelConfig()
model_config_new.CopyFrom(model_config)
model_config_new.inner_iterations_count = 20

dirichlet_regularizer_config_new = messages_pb2.DirichletThetaConfig()
alpha = dirichlet_regularizer_config_new.alpha.add()
alpha.value.append(0.2)
general_dirichlet_config.config = dirichlet_regularizer_config_new.SerializeToString()

dictionary_config = messages_pb2.DictionaryConfig()
dictionary_config.name = 'dictionary_1'

entry_1 = dictionary_config.entry.add()
entry_1.key_token = 'token_1'
entry_1.value = 0.4
entry_2 = dictionary_config.entry.add()
entry_2.key_token = 'token_2'
entry_2.value = 0.6

general_phi_regularizer = messages_pb2.RegularizerConfig()
general_phi_regularizer.name = 'regularizer_2'
general_phi_regularizer.type = 1
general_phi_regularizer.dictionary_name.append('dictionary_1')

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

with library.CreateMasterComponent() as master_component:
  master_component.Reconfigure(master_config)
  master_component.AddStream(stream)
  master_component.RemoveStream(stream)
  model = master_component.CreateModel(model_config)
  master_component.RemoveModel(model)
  model = master_component.CreateModel(model_config)

  regularizer = master_component.CreateRegularizer(general_dirichlet_config)
  master_component.RemoveRegularizer(regularizer)
  regularizer = master_component.CreateRegularizer(general_dirichlet_config)

  dictionary = master_component.CreateDictionary(dictionary_config)
  regularizer_phi = master_component.CreateRegularizer(general_phi_regularizer)

  master_component.AddBatch(batch)
  model.Enable()
  master_component.InvokeIteration(10)
  model.Disable()
  topic_model = master_component.GetTopicModel(model)
  theta_matrix = master_component.GetThetaMatrix(model)

  # Test all 'reconfigure' methods
  regularizer.Reconfigure(general_dirichlet_config)
  model.Reconfigure(model_config_new)
  master_component.Reconfigure(master_config_new)

  master_component.RemoveDictionary(dictionary)

print 'All tests have been successfully passed!'
