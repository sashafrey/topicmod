// Copyright 2014, Additive Regularization of Topic Models.
// This file defines public API methods of BigARTM library.
// All methods must be inside "extern "C"" scope. All complex data structures should be passed in
// as Google Protobuf Messages, defined in artm/messages.pb.c

#ifndef SRC_ARTM_C_INTERFACE_H_
#define SRC_ARTM_C_INTERFACE_H_

#if defined(_WIN32) || defined(_WIN64)
  #ifdef ARTM_DLL_EXPORTS
    #define DLL_PUBLIC __declspec(dllexport)
  #else
    #define DLL_PUBLIC __declspec(dllimport)
  #endif
#else
  #define DLL_PUBLIC
#endif

extern "C" {
  DLL_PUBLIC enum ArtmErrorCodes {
    ARTM_SUCCESS = 0,
    ARTM_GENERAL_ERROR = -1,
    ARTM_OBJECT_NOT_FOUND = -2,
    ARTM_INVALID_MESSAGE = -3,
    ARTM_INVALID_OPERATION = -4,
    ARTM_NETWORK_ERROR = -5,
  };

  // ===============================================================================================
  // Common routines
  // ===============================================================================================
  DLL_PUBLIC int ArtmConfigureLogger(int length, const char* logger_config);

  DLL_PUBLIC int ArtmGetRequestLength(int request_id);
  DLL_PUBLIC int ArtmCopyRequestResult(int request_id, int length, char* address);
  DLL_PUBLIC void ArtmDisposeRequest(int request_id);

  // ===============================================================================================
  // MasterComponent interface
  // ===============================================================================================
  DLL_PUBLIC int ArtmCreateMasterComponent(int master_id, int length, const char* config_blob);
  DLL_PUBLIC int ArtmReconfigureMasterComponent(int master_id, int length, const char* config);
  DLL_PUBLIC void ArtmDisposeMasterComponent(int master_id);

  DLL_PUBLIC int ArtmCreateNodeController(int node_controller_id, int length,
                                          const char* config_blob);
  DLL_PUBLIC void ArtmDisposeNodeController(int node_controller_id);

  DLL_PUBLIC int ArtmCreateModel(int master_id, int length, const char* model_config_blob);
  DLL_PUBLIC int ArtmReconfigureModel(int master_id, int length, const char* config);
  DLL_PUBLIC void ArtmDisposeModel(int master_id, const char* model_name);

  DLL_PUBLIC int ArtmCreateRegularizer(int master_id, int length,
                                       const char* regularizer_config_blob);
  DLL_PUBLIC int ArtmReconfigureRegularizer(int master_id, int length,
                                            const char* regularizer_config_blob);
  DLL_PUBLIC void ArtmDisposeRegularizer(int master_id, const char* regularizer_name);

  DLL_PUBLIC int ArtmAddBatch(int master_id, int length, const char* batch_blob);
  DLL_PUBLIC int ArtmInvokeIteration(int master_id, int iterations_count);
  DLL_PUBLIC int ArtmInvokePhiRegularizers(int master_id);
  DLL_PUBLIC int ArtmWaitIdle(int master_id);

  DLL_PUBLIC int ArtmRequestThetaMatrix(int master_id, const char* model_name);
  DLL_PUBLIC int ArtmRequestTopicModel(int master_id, const char* model_name);
  DLL_PUBLIC int ArtmOverwriteTopicModel(int master_id, int length, const char* topic_model_blob);
}

#endif  // SRC_ARTM_C_INTERFACE_H_
