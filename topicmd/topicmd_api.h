#ifndef __TOPICMD_API
#define __TOPICMD_API

#define TOPICMD_ERROR -1
#define TOPICMD_SUCCESS 0

extern "C" {

int create_topicmd_instance(int length, const byte* buffer_configuration);
int update_topicmd_instance(int instance_id, int length, const byte* buffer_configuration);
void destroy_topicmd_instance(int instance_id);

int insert_items(int instance_id, int length, const byte* buffer_items_vector);
int publish_items(int instance_id, int generation_id);
int commit_items(int instance_id, int generation_id);

int process_items(int instance_id, int length, const byte* buffer_ids_vector);
int process_all_indexed_items(int instance_id);

int configure_logger(int length, const byte* logger_configuration);

int request_word_topic_matrix(int instance_id, int *length, byte **address);

int copy_request_result(int request_id, int length, byte* address);
int finish_request(int request_id);

}

#endif // __TOPICMD_API
