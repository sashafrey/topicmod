#ifndef __TOPICMD_API
#define __TOPICMD_API

#define TOPICMD_ERROR -1
#define TOPICMD_SUCCESS 0

extern "C" {

  int commit_generation(int instance_id, int generation_id);

  int complete_request(int request_id);

  int configure_logger(int length, const byte* logger_config);

  int copy_request_result(int request_id, int length, byte* address);

  int create_instance(int instance_id, 
		      int length, 
		      const byte* instance_config_blob);

  int create_model(int instance_id,
		   int model_id,
		   int length, 
		   const byte* model_config_blob);

  void destroy_instance(int instance_id);

  void destroy_model(int instance_id, int model_id);

  int finish_generation(int instance_id);

  int insert_batch(int instance_id, int length, const byte* batch_blob);

  int publish_generation(int instance_id, int generation_id);

  int reconfigure_instance(int instance_id,
			   int length, 
			   const byte* instance_config_blob);

  int reconfigure_model(int instance_id,
			int model_id,
			int length,
			const byte* model_config_blob);

  int request_batch_topics(int instance_id,
		     int model_id,
		     int batch_length,
		     const byte* batch_blob,
		     int *length,
		     const byte** result);	    

  int request_model_topics(int instance_id,
		    int model_id,
		    int *length,
		    byte **address);

  int run_tuning_iteration(int instance_id);

}

#endif // __TOPICMD_API
