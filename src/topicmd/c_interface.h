#ifndef TOPICMD_H_
#define TOPICMD_H_

#define TOPICMD_ERROR -1
#define TOPICMD_SUCCESS 0

extern "C" {
  int commit_generation(int instance_id, int generation_id);

  int configure_logger(int length, const char* logger_config);

  int copy_request_result(int request_id, int length, char* address);

  int create_instance(int instance_id, 
		      int length, 
		      const char* instance_config_blob);

  int create_model(int instance_id,
		   int model_id,
		   int length, 
		   const char* model_config_blob);

  void dispose_instance(int instance_id);

  void dispose_model(int instance_id, int model_id);

  void dispose_request(int request_id);

  int finish_generation(int instance_id);

  int insert_batch(int instance_id, int length, const char* batch_blob);

  int publish_generation(int instance_id, int generation_id);

  int reconfigure_instance(int instance_id,
			   int length, 
			   const char* instance_config_blob);

  int reconfigure_model(int instance_id,
			int model_id,
			int length,
			const char* model_config_blob);

  int request_batch_topics(int instance_id,
		     int model_id,
		     int batch_length,
		     const char* batch_blob,
		     int *length,
		     const char** result);	    

  int request_model_topics(int instance_id,
		    int model_id,
		    int *length,
		    char **address);

  int run_tuning_iteration(int instance_id);
}


#endif // TOPICMD_H_
