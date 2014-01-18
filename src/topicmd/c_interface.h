#ifndef TOPICMD_H_
#define TOPICMD_H_

#if defined _WIN32
	#ifdef TOPICMD_DLL_EXPORTS
		#define DLL_PUBLIC __declspec(dllexport)
	#else
		#define DLL_PUBLIC __declspec(dllimport)
	#endif
#else
	#define DLL_PUBLIC
#endif

extern "C" {
  DLL_PUBLIC int commit_generation(int instance_id, int generation_id);

  DLL_PUBLIC int configure_logger(int length, const char* logger_config);

  DLL_PUBLIC int copy_request_result(int request_id, int length, char* address);

  DLL_PUBLIC
  int create_instance(int instance_id, 
		      int length, 
		      const char* instance_config_blob);

  DLL_PUBLIC
	int create_model(int instance_id,
		   int model_id,
		   int length, 
		   const char* model_config_blob);

  DLL_PUBLIC int discard_partition(int instance_id);

  DLL_PUBLIC void dispose_instance(int instance_id);

  DLL_PUBLIC void dispose_model(int instance_id, int model_id);

  DLL_PUBLIC void dispose_request(int request_id);

  DLL_PUBLIC int finish_partition(int instance_id);

  DLL_PUBLIC int insert_batch(int instance_id, int length, const char* batch_blob);

  DLL_PUBLIC int publish_generation(int instance_id, int generation_id);

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

  DLL_PUBLIC int run_tuning_iteration(int instance_id);
}


#endif // TOPICMD_H_
