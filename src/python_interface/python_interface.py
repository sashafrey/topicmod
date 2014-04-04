# Copyright 2014, Additive Regularization of Topic Models.

# Author: Murat Apishev (great-mel@yandex.ru)
 
import messages_pb2
import os
from os import *
import ctypes
from ctypes import *

#################################################################################

ARTM_SUCCESS = 0
ARTM_GENERAL_ERROR = -1
ARTM_OBJECT_NOT_FOUND = -2
ARTM_INVALID_MESSAGE = -3
ARTM_UNSUPPORTED_RECONFIGURATION = -4
address = os.path.abspath(os.path.join(os.curdir, os.pardir))
lib = ctypes.CDLL(address + '\\Win32\\Debug\\artm_dll.dll')

#################################################################################

class GeneralError(BaseException) : pass
class ObjectNotFound(BaseException) : pass
class InvalidMessage(BaseException) : pass
class UnsupportedReconfiguration(BaseException) : pass

def HandleErrorCode(artm_error_code):
  if (artm_error_code == ARTM_SUCCESS) | (artm_error_code >= 0): 
    return artm_error_code
  elif artm_error_code == ARTM_OBJECT_NOT_FOUND:
    raise ObjectNotFound()
  elif artm_error_code == ARTM_INVALID_MESSAGE:
    raise InvalidMessage()
  elif artm_error_code == ARTM_UNSUPPORTED_RECONFIGURATION:
    raise UnsupportedReconfiguration()
  elif artm_error_code == ARTM_GENERAL_ERROR:
    raise GeneralError()
  else:
    raise GeneralError()

#################################################################################

class Instance:
  def __init__(self, config):
    self.config_ = config       
    instance_config_blob = config.SerializeToString()
    instance_config_blob_p = ctypes.create_string_buffer(instance_config_blob)
    self.id_ = HandleErrorCode(lib.ArtmCreateInstance(0, 
               len(instance_config_blob), instance_config_blob_p));
  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    lib.ArtmDisposeInstance(self.id_)

  def Reconfigure(self, config):
    config_blob = config.SerializeToString()
    config_blob_p = ctypes.create_string_buffer(config_blob)
    HandleErrorCode(lib.ArtmReconfigureInstance(self.id_, len(config_blob), config_blob_p));
    self.config_.CopyFrom(config)
             
  def GetTopics(self, model):
    request_id = HandleErrorCode(lib.ArtmRequestModelTopics(self.id_, model.model_id_)); 
    length = HandleErrorCode(lib.ArtmGetRequestLength(request_id))

    model_topics_blob = ctypes.create_string_buffer(length)
    HandleErrorCode(lib.ArtmCopyRequestResult(request_id, length, model_topics_blob));
    lib.ArtmDisposeRequest(request_id)
     
    model_topics = messages_pb2.ModelTopics()
    model_topics.ParseFromString(model_topics_blob)
    return model_topics

#################################################################################

class Model:
  def __init__(self, instance, config):
    self.instance_id_ = instance.id_
    self.config_ = config
    model_config_blob = config.SerializeToString()
    model_config_blob_p = ctypes.create_string_buffer(model_config_blob)
    self.model_id_ = HandleErrorCode(lib.ArtmCreateModel(self.instance_id_, 
                     len(model_config_blob), model_config_blob_p));
        
  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    lib.ArtmDisposeModel(self.instance_id_, self.model_id_)

  def Reconfigure(self, config):
    model_config_blob = config.SerializeToString()
    model_config_blob_p = ctypes.create_string_buffer(model_config_blob)
    HandleErrorCode(lib.ArtmReconfigureModel(self.instance_id_, self.model_id_, 
                    len(model_config_blob), model_config_blob_p));
    self.config_.CopyFrom(config)  

  def Enable(self):
    config_copy_ = messages_pb2.ModelConfig()
    config_copy_.CopyFrom(self.config_)
    config_copy_.enabled = True
    self.Reconfigure(config_copy_)

  def Disable(self):
    config_copy_ = messages_pb2.ModelConfig()
    config_copy_.CopyFrom(self.config_)
    config_copy_.enabled = False
    self.Reconfigure(config_copy_)

#################################################################################

class DataLoader:
  def __init__(self, instance, config):
    self.config_ = config
    self.config_.instance_id = instance.id_
    data_loader_config_blob = self.config_.SerializeToString()
    data_loader_config_blob_p = ctypes.create_string_buffer(data_loader_config_blob)
    self.id_ = HandleErrorCode(lib.ArtmCreateDataLoader(0, 
               len(data_loader_config_blob), data_loader_config_blob_p))

  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    lib.ArtmDisposeDataLoader(self.id_)
        
  def AddBatch(self, batch):
    batch_blob = batch.SerializeToString()
    batch_blob_p = ctypes.create_string_buffer(batch_blob)
    HandleErrorCode(lib.ArtmAddBatch(self.id_, len(batch_blob), batch_blob_p))

  def Reconfigure(self, config):
    config_blob = config.SerializeToString()
    config_blob_p = ctypes.create_string_buffer(config_blob)
    HandleErrorCode(lib.ArtmReconfigureDataLoader(self.id_, len(config_blob), config_blob_p))
    self.config_.CopyFrom(config)

  def InvokeIteration(self, iterations_count):
    HandleErrorCode(lib.ArtmInvokeIteration(self.id_, iterations_count))

  def WaitIdle(self):
    HandleErrorCode(lib.ArtmWaitIdleDataLoader(self.id_))

  def AddStream(self, stream):
    s = self.config_.stream.add()
    s.CopyFrom(stream)
    self.Reconfigure(self.config_)

  def RemoveStream(self, stream_name):
    new_config_ = messages_pb2.DataLoaderConfig()
    new_config_.CopyFrom(self.config_)
    new_config_.ClearField('stream')
        
    for stream_index in range(0, len(self.config_.stream)):
      if (self.config_.stream[stream_index].name != stream_name):
        s = new_config_.stream.add()
        s.CopyFrom(self.config_.stream[stream_index])
    self.Reconfigure(new_config_)