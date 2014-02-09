#ifndef TOPICMD_H_
#define TOPICMD_H_

#ifdef _WIN32
  #ifdef TOPICMD_DLL_EXPORTS
    #define DLL_PUBLIC __declspec(dllexport)
  #else
    #define DLL_PUBLIC __declspec(dllimport)
  #endif
#else
  #define DLL_PUBLIC
#endif

extern "C" {
  // =========================================================================
  // Common routines
  // =========================================================================

  DLL_PUBLIC int configure_logger(int length, const char* logger_config);

  DLL_PUBLIC int copy_request_result(int request_id, int length, char* address);

  // =========================================================================
  // Data loader interface
  // =========================================================================

  DLL_PUBLIC 
  int create_data_loader(int data_loader_id, 
                         int length, 
                         const char* data_loader_config_blob);

  DLL_PUBLIC 
  void dispose_data_loader(int data_loader_id);

  DLL_PUBLIC 
  int add_batch(int data_loader_id, int length, const char* batch_blob);

  // =========================================================================
  // Instance interface
  // =========================================================================

  DLL_PUBLIC
  int create_instance(int instance_id, 
                      int length, 
                      const char* instance_config_blob);

  DLL_PUBLIC
  int create_model(int instance_id,
                   int model_id,
                   int length, 
                   const char* model_config_blob);

  DLL_PUBLIC void dispose_instance(int instance_id);

  DLL_PUBLIC void dispose_model(int instance_id, int model_id);

  DLL_PUBLIC void dispose_request(int request_id);

  DLL_PUBLIC int reconfigure_instance(int instance_id,
                                      int length, 
                                      const char* instance_config_blob);

  DLL_PUBLIC int reconfigure_model(int instance_id,
                                   int model_id,
                                   int length,
                                   const char* model_config_blob);

  DLL_PUBLIC int request_batch_topics(int instance_id,
                                      int model_id,
                                      int batch_length,
                                      const char* batch_blob,
                                      int *length,
                                      const char** result);      

  DLL_PUBLIC int request_model_topics(int instance_id,
                                      int model_id,
                                      int *length,
                                      char **address);

  DLL_PUBLIC int wait_model_processed(int instance_id,
                                      int model_id,
                                      int processed_items);
}


#endif // TOPICMD_H_
