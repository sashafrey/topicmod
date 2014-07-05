# Copyright 2014, Additive Regularization of Topic Models.

import messages_pb2
import os
import ctypes
import uuid
from ctypes import *

#################################################################################

ARTM_SUCCESS = 0
ARTM_GENERAL_ERROR = -1
ARTM_OBJECT_NOT_FOUND = -2
ARTM_INVALID_MESSAGE = -3
ARTM_INVALID_OPERATION = -4

Stream_Type_Global = 0
Stream_Type_ItemIdModulus = 1
RegularizerConfig_Type_DirichletTheta = 0
RegularizerConfig_Type_DirichletPhi = 1
RegularizerConfig_Type_SmoothSparseTheta = 2
RegularizerConfig_Type_SmoothSparsePhi = 3
Score_Type_Perplexity = 0

#################################################################################

class GeneralError(BaseException) : pass
class ObjectNotFound(BaseException) : pass
class InvalidMessage(BaseException) : pass
class InvalidOperation(BaseException) : pass

def HandleErrorCode(artm_error_code):
  if (artm_error_code == ARTM_SUCCESS) | (artm_error_code >= 0):
    return artm_error_code
  elif artm_error_code == ARTM_OBJECT_NOT_FOUND:
    raise ObjectNotFound()
  elif artm_error_code == ARTM_INVALID_MESSAGE:
    raise InvalidMessage()
  elif artm_error_code == ARTM_INVALID_OPERATION:
    raise InvalidOperation()
  elif artm_error_code == ARTM_GENERAL_ERROR:
    raise GeneralError()
  else:
    raise GeneralError()

#################################################################################

class ArtmLibrary:
  def __init__(self, location):
    self.lib_ = ctypes.CDLL(location)

  def CreateMasterComponent(self, config=messages_pb2.MasterComponentConfig()):
    return MasterComponent(config, self.lib_)

  def CreateNodeController(self, endpoint):
    config = messages_pb2.NodeControllerConfig();
    config.create_endpoint = endpoint;
    return NodeController(config, self.lib_)

#################################################################################

class MasterComponent:
  def __init__(self, config, lib):
    self.lib_ = lib
    master_config_blob = config.SerializeToString()
    master_config_blob_p = ctypes.create_string_buffer(master_config_blob)

    if (isinstance(config, messages_pb2.MasterComponentConfig)):
      self.config_ = config
      self.id_ = HandleErrorCode(self.lib_.ArtmCreateMasterComponent(0,
                 len(master_config_blob), master_config_blob_p))
      return

    if (isinstance(config, messages_pb2.MasterProxyConfig)):
      self.config_ = config.config
      self.id_ = HandleErrorCode(self.lib_.ArtmCreateMasterProxy(0,
                 len(master_config_blob), master_config_blob_p))
      return

    raise InvalidOperation()

  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    self.Dispose()

  def Dispose(self):
    self.lib_.ArtmDisposeMasterComponent(self.id_)
    self.id_ = -1

  def CreateModel(self, config):
    return Model(self, config)

  def RemoveModel(self, model):
    model.__Dispose__()

  def CreateRegularizer(self, name, type, config):
    general_config = messages_pb2.RegularizerConfig()
    general_config.name = name
    general_config.type = type
    general_config.config = config.SerializeToString()
    return Regularizer(self, general_config)

  def RemoveRegularizer(self, regularizer):
    regularizer.__Dispose__()

  def CreateDictionary(self, config):
    return Dictionary(self, config)

  def RemoveDictionary(self, dictionary):
    dictionary.__Dispose__()

  def Reconfigure(self, config):
    config_blob = config.SerializeToString()
    config_blob_p = ctypes.create_string_buffer(config_blob)
    HandleErrorCode(self.lib_.ArtmReconfigureMasterComponent(self.id_, len(config_blob), config_blob_p))
    self.config_.CopyFrom(config)

  def AddBatch(self, batch):
    batch_blob = batch.SerializeToString()
    batch_blob_p = ctypes.create_string_buffer(batch_blob)
    HandleErrorCode(self.lib_.ArtmAddBatch(self.id_, len(batch_blob), batch_blob_p))

  def InvokeIteration(self, iterations_count):
    HandleErrorCode(self.lib_.ArtmInvokeIteration(self.id_, iterations_count))

  def WaitIdle(self):
    HandleErrorCode(self.lib_.ArtmWaitIdle(self.id_))

  def AddStream(self, stream):
    s = self.config_.stream.add()
    s.CopyFrom(stream)
    self.Reconfigure(self.config_)

  def RemoveStream(self, stream_name):
    new_config_ = messages_pb2.MasterComponentConfig()
    new_config_.CopyFrom(self.config_)
    new_config_.ClearField('stream')

    for stream_index in range(0, len(self.config_.stream)):
      if (self.config_.stream[stream_index].name != stream_name):
        s = new_config_.stream.add()
        s.CopyFrom(self.config_.stream[stream_index])
    self.Reconfigure(new_config_)

  def GetTopicModel(self, model):
    request_id = HandleErrorCode(self.lib_.ArtmRequestTopicModel(self.id_, model.name()))
    length = HandleErrorCode(self.lib_.ArtmGetRequestLength(request_id))

    topic_model_blob = ctypes.create_string_buffer(length)
    HandleErrorCode(self.lib_.ArtmCopyRequestResult(request_id, length, topic_model_blob))
    self.lib_.ArtmDisposeRequest(request_id)

    topic_model = messages_pb2.TopicModel()
    topic_model.ParseFromString(topic_model_blob)
    return topic_model

  def GetThetaMatrix(self, model):
    request_id = HandleErrorCode(self.lib_.ArtmRequestThetaMatrix(self.id_, model.name()))
    length = HandleErrorCode(self.lib_.ArtmGetRequestLength(request_id))

    blob = ctypes.create_string_buffer(length)
    HandleErrorCode(self.lib_.ArtmCopyRequestResult(request_id, length, blob))
    self.lib_.ArtmDisposeRequest(request_id)

    theta_matrix = messages_pb2.ThetaMatrix()
    theta_matrix.ParseFromString(blob)
    return theta_matrix

#################################################################################

class NodeController:
  def __init__(self, config, lib):
    self.lib_ = lib
    config_blob = config.SerializeToString()
    config_blob_p = ctypes.create_string_buffer(config_blob)

    self.id_ = HandleErrorCode(self.lib_.ArtmCreateNodeController(0,
               len(config_blob), config_blob_p))

  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    self.Dispose()

  def Dispose(self):
    self.lib_.ArtmDisposeNodeController(self.id_)
    self.id_ = -1

#################################################################################

class Model:
  def __init__(self, master_component, config):
    self.lib_ = master_component.lib_
    self.master_id_ = master_component.id_
    self.config_ = config
    self.config_.name = uuid.uuid1().urn
    model_config_blob = config.SerializeToString()
    model_config_blob_p = ctypes.create_string_buffer(model_config_blob)
    HandleErrorCode(self.lib_.ArtmCreateModel(self.master_id_,
                    len(model_config_blob), model_config_blob_p))

  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    __Dispose__(self)

  def __Dispose__(self):
    self.lib_.ArtmDisposeModel(self.master_id_, self.config_.name)
    self.config_.name = ''
    self.master_id_ = -1

  def name(self):
    return self.config_.name

  def Reconfigure(self, config):
    model_config_blob = config.SerializeToString()
    model_config_blob_p = ctypes.create_string_buffer(model_config_blob)
    HandleErrorCode(self.lib_.ArtmReconfigureModel(self.master_id_,
                    len(model_config_blob), model_config_blob_p))
    self.config_.CopyFrom(config)

  def InvokePhiRegularizers(self):
    HandleErrorCode(self.lib_.ArtmInvokePhiRegularizers(self.master_id_))

  def Overwrite(self, topic_model):
    blob = topic_model.SerializeToString()
    blob_p = ctypes.create_string_buffer(blob)
    HandleErrorCode(self.lib_.ArtmOverwriteTopicModel(self.master_id_, len(blob), blob_p))

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

class Regularizer:
  def __init__(self, master_component, config):
    self.lib_ = master_component.lib_
    self.master_id_ = master_component.id_
    self.config_ = config
    regularizer_config_blob = config.SerializeToString()
    regularizer_config_blob_p = ctypes.create_string_buffer(regularizer_config_blob)
    HandleErrorCode(self.lib_.ArtmCreateRegularizer(self.master_id_,
                     len(regularizer_config_blob), regularizer_config_blob_p))

  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    __Dispose__(self)

  def __Dispose__(self):
    self.lib_.ArtmDisposeRegularizer(self.master_id_, self.config_.name)
    self.config_.name = ''
    self.master_id_ = -1

  def name(self):
    return self.config_.name

  def Reconfigure(self, type, config):
    general_config = messages_pb2.RegularizerConfig()
    general_config.name = self.name()
    general_config.type = type
    general_config.config = config.SerializeToString()

    regularizer_config_blob = general_config.SerializeToString()
    regularizer_config_blob_p = ctypes.create_string_buffer(regularizer_config_blob)
    HandleErrorCode(self.lib_.ArtmReconfigureRegularizer(self.master_id_,
                    len(regularizer_config_blob), regularizer_config_blob_p))
    self.config_.CopyFrom(general_config)

#################################################################################

class Dictionary:
  def __init__(self, master_component, config):
    self.lib_ = master_component.lib_
    self.master_id_ = master_component.id_
    self.config_ = config
    dictionary_config_blob = config.SerializeToString()
    dictionary_config_blob_p = ctypes.create_string_buffer(dictionary_config_blob)
    HandleErrorCode(self.lib_.ArtmCreateDictionary(self.master_id_,
                     len(dictionary_config_blob), dictionary_config_blob_p))

  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    __Dispose__(self)

  def __Dispose__(self):
    self.lib_.ArtmDisposeDictionary(self.master_id_, self.config_.name)
    self.config_.name = ''
    self.master_id_ = -1

  def name(self):
    return self.config_.name

  def Reconfigure(self, config):
    dictionary_config_blob = config.SerializeToString()
    dictionary_config_blob_p = ctypes.create_string_buffer(dictionary_config_blob)
    HandleErrorCode(self.lib_.ArtmReconfigureDictionary(self.master_id_,
                    len(dictionary_config_blob), dictionary_config_blob_p))
    self.config_.CopyFrom(config)
