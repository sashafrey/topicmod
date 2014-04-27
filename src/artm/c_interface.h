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
    ARTM_UNSUPPORTED_RECONFIGURATION = -4,
    ARTM_NETWORK_ERROR = -5,
  };

  // ===============================================================================================
  // Common routines
  // ===============================================================================================
  DLL_PUBLIC int ArtmConfigureLogger(int length, const char* logger_config);
  DLL_PUBLIC int ArtmCopyRequestResult(int request_id, int length, char* address);
  DLL_PUBLIC int ArtmGetRequestLength(int request_id);

  // ===============================================================================================
  // Memcached service - host
  // ===============================================================================================
  DLL_PUBLIC int ArtmCreateMemcachedServer(const char* endpoint);
  DLL_PUBLIC int ArtmDisposeMemcachedServer(int memcached_server_id);

  // ===============================================================================================
  // Data loader interface
  // ===============================================================================================
  DLL_PUBLIC int ArtmAddBatch(int data_loader_id, int length, const char* batch_blob);
  DLL_PUBLIC int ArtmCreateDataLoader(int data_loader_id, int length, const char* config);
  DLL_PUBLIC int ArtmInvokeIteration(int data_loader_id, int iterations_count);
  DLL_PUBLIC int ArtmReconfigureDataLoader(int data_loader_id, int length, const char* config);
  DLL_PUBLIC int ArtmWaitIdleDataLoader(int data_loader_id);
  DLL_PUBLIC void ArtmDisposeDataLoader(int data_loader_id);

  // ===============================================================================================
  // Instance interface
  // ===============================================================================================
  DLL_PUBLIC int ArtmCreateInstance(int instance_id, int length, const char* instance_config_blob);
  DLL_PUBLIC int ArtmCreateModel(int instance_id, int length, const char* model_config_blob);
  DLL_PUBLIC int ArtmReconfigureInstance(int instance_id, int length, const char* config);
  DLL_PUBLIC int ArtmReconfigureModel(int instance_id, int model_id, int length,
                                      const char* config);
  DLL_PUBLIC int ArtmRequestBatchTopics(int instance_id, int model_id, int batch_length,
                                        const char* batch_blob);
  DLL_PUBLIC int ArtmRequestModelTopics(int instance_id, int model_id);

  DLL_PUBLIC void ArtmDisposeInstance(int instance_id);
  DLL_PUBLIC void ArtmDisposeModel(int instance_id, int model_id);
  DLL_PUBLIC void ArtmDisposeRequest(int request_id);

  DLL_PUBLIC int ArtmCreateRegularizer(int instance_id, int length,
                                       const char* regularizer_config_blob);
  DLL_PUBLIC int ArtmReconfigureRegularizer(int instance_id, int length,
                                            const char* regularizer_config_blob);
  DLL_PUBLIC void ArtmDisposeRegularizer(int instance_id, const char* regularizer_name);
}

#endif  // SRC_ARTM_C_INTERFACE_H_
