# Copyright 2014, Additive Regularization of Topic Models.

# Author: Murat Apishev (great-mel@yandex.ru)

import messages_pb2
import python_interface
from python_interface import *

#################################################################################
# ALL CODE BELOW DEFINES PROTOBUF MESSAGES NEED TO TEST THE INTERFACE FUNCTIONS

data_loader_config = messages_pb2.DataLoaderConfig()
data_loader_config.instance_id = 1
data_loader_config.disk_path = ('D:\\')
data_loader_config.queue_size = 5
stream_ = data_loader_config.stream.add()
stream_.name = ('stream_1')
stream_.type = 0
stream_.modulus = 4
stream_.residuals.append(2)

data_loader_config_new = messages_pb2.DataLoaderConfig()
data_loader_config_new.instance_id = 2
data_loader_config_new.disk_path = ('C:\\')
data_loader_config_new.queue_size = 9
stream_ = data_loader_config_new.stream.add()
stream_.name = ('stream_0')
stream_.type = 0
stream_.modulus = 3
stream_.residuals.append(9)

stream = messages_pb2.Stream()
stream.name = ('stream_8')
stream.type = 0
stream.modulus = 3
stream.residuals.append(1)

batch = messages_pb2.Batch()
batch.token.append('first')
item_ = batch.item.add()
item_.id = 2
field_ = item_.field.add()
field_.token_id.append(0)
field_.token_count.append(2)

model_config = messages_pb2.ModelConfig()
model_config.stream_name = ('stream_0')
score_ = model_config.score.add()
score_.type = 0
score_.stream_name = ('stream_0')

model_config_new = messages_pb2.ModelConfig()
model_config_new.stream_name = ('stream_1')
score_ = model_config.score.add()
score_.type = 0
score_.stream_name = ('stream_1')

instance_config = messages_pb2.InstanceConfig()
instance_config.processors_count = 2

instance_config_new = messages_pb2.InstanceConfig()
instance_config_new.processors_count = 1

#################################################################################
# TEST SECTION

address = os.path.abspath(os.path.join(os.curdir, os.pardir))
os.environ['PATH'] = ';'.join([address + '\\Win32\\Debug', os.environ['PATH']])
library = ArtmLibrary(address + '\\Win32\\Debug\\artm.dll')

instance = library.CreateInstance(instance_config)
instance.Reconfigure(instance_config_new)
model = library.CreateModel(instance, model_config)
instance.GetTopics(model)
model.Reconfigure(model_config_new)
model.Disable()
model.Enable()
data_loader = library.CreateDataLoader(instance, data_loader_config)
data_loader.AddBatch(batch)
data_loader.Reconfigure(data_loader_config_new)
data_loader.InvokeIteration(10)
data_loader.AddStream(stream)
data_loader.RemoveStream('stream_8')
#data_loader.WaitIdle()

print 'All tests have been successfully passed!'