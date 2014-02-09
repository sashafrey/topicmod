#include <ctime>
#include <cstring>
#include <iostream>

using namespace std;

#include "doc_token_matrix.h"
#include "token_topic_matrix.h"
#include "doc_topic_matrix.h"
#include "vocab.h"
#include "helpers.h"

// topicmd library
#include "topicmd/c_interface.h"
#include "topicmd/messages.pb.h"

using namespace topicmd;

inline char* string_as_array(string* str) {
  return str->empty() ? NULL : &*str->begin();
}

double proc(int argc, char * argv[], int processors_count) {
  // Create instance
  InstanceConfig instance_config;
  instance_config.set_processors_count(processors_count);
  string instance_config_blob;
  instance_config.SerializeToString(&instance_config_blob);
  int instance_id = 
    create_instance(
      0,
      instance_config_blob.size(), 
      string_as_array(&instance_config_blob));

  // Create data loader
  DataLoaderConfig data_loader_config;
  data_loader_config.set_instance_id(instance_id);
  string data_loader_config_blob;
  instance_config.SerializeToString(&instance_config_blob);
  data_loader_config.SerializeToString(&data_loader_config_blob);
  int data_loader_id =
    create_data_loader(
      0,
      data_loader_config_blob.size(),
      string_as_array(&data_loader_config_blob));

  // Create model
  int nTopics = atoi(argv[3]);
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
  model_config.set_inner_iterations_count(10);
  string model_config_blob;
  model_config.SerializeToString(&model_config_blob);
  int model_id = 
    create_model(
      instance_id, 
      0,
      model_config_blob.size(), 
      string_as_array(&model_config_blob));
  
  // Load doc-word matrix
  DocWordMatrix::Ptr pD = loadMatrixFileUCI(argv[1]);
  VocabPtr pVocab = loadVocab(argv[2], pD->getW());
  int nWords = pD->getW();
  int nDocs = pD->getD();

  int nParts = 16; 
  int iDoc = 0; 
  int docsPerPart = nDocs / nParts + 1;
  for (int iPart = 1; iPart <= nParts; iPart++) 
  {
    Batch batch;
    for (int i = 0; i < nWords; i++) {
      batch.add_token((*pVocab)[i]);
    }

    for (; iDoc < (docsPerPart * iPart) && (iDoc < nDocs) ; iDoc++) {
      auto term_ids = pD->getTermId(iDoc);
      auto term_counts = pD->getFreq(iDoc);

      Item* item = batch.add_item();
      Field* field = item->add_field();
      for (int iWord = 0; iWord < (int)term_ids.size(); ++iWord) {
        field->add_token_id(term_ids[iWord]);
        field->add_token_count((google::protobuf::int32) term_counts[iWord]);
      }
    }

    // Index doc-word matrix
    string batch_blob;
    batch.SerializeToString(&batch_blob);
    add_batch(data_loader_id, batch_blob.size(), string_as_array(&batch_blob));
  }

  clock_t begin = clock();
  // enable model
  model_config.set_enabled(true);
  model_config.SerializeToString(&model_config_blob);
  reconfigure_model(instance_id, model_id, model_config_blob.size(), string_as_array(&model_config_blob));

  // wait while each document pass through processor about 100 times.
  wait_model_processed(instance_id, model_id, nDocs * 10);

  // disable model
  model_config.set_enabled(false);
  model_config.SerializeToString(&model_config_blob);
  reconfigure_model(instance_id, model_id, model_config_blob.size(), string_as_array(&model_config_blob));

  clock_t end = clock();

  // Request model topics
  int length;
  char* address;
  int request_id = 
    request_model_topics(
      instance_id, 
      model_id, 
      &length,
      &address);

  string model_topics_blob;
  model_topics_blob.resize(length);
  copy_request_result(
    request_id, 
    length, 
    string_as_array(&model_topics_blob));

  ModelTopics model_topics;
  model_topics.ParseFromString(model_topics_blob);
  // request_model_topics(instance_id, model_id, &model_topics);

  std::cout << "Number of tokens in model topic: " 
            << model_topics.token_topic_size()
            << endl;

  // dispose_request(request_id);
  dispose_model(instance_id, model_id);
  dispose_data_loader(data_loader_id);
  dispose_instance(instance_id);
  

  // Log top 7 words per each topic
  {
    int wordsToSort = 7;
    int nTokens = model_topics.token_topic_size();
    int nTopics = model_topics.token_topic(0).topic_weight_size();

    for (int iTopic = 0; iTopic < nTopics; iTopic++) {
      std::cout << "#" << (iTopic+1) << ": ";

      std::vector<std::pair<float, std::string> > p_w;
      for (int iToken = 0; iToken < nTokens; ++iToken) {
        const TokenTopics& token_topic = model_topics.token_topic(iToken);
        string token = token_topic.token();
        float weight = token_topic.topic_weight(iTopic);
          p_w.push_back(std::pair<float, std::string>(weight, token));
      }

        std::sort(p_w.begin(), p_w.end());
        for (int iWord = (int)p_w.size() - 1;
             (iWord >= 0) && (iWord >= (int)p_w.size() - wordsToSort);
             iWord--)
        {
          std::cout << p_w[iWord].second << " ";
        }
      
      std::cout << std::endl;
    }
  }

  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  return elapsed_secs;
}

int main(int argc, char * argv[]) {
  if (argc != 4) {
    cout << "Usage: ./PlsaBatchEM <docword> <vocab> nTopics" << endl;
    return 0;
  }

//  cout << proc(argc, argv, 4) << " sec. ================= " << endl << endl;
 // cout << proc(argc, argv, 3) << " sec. ================= " << endl << endl;
  cout << proc(argc, argv, 2) << " sec. ================= " << endl << endl;
  //cout << proc(argc, argv, 1) << " sec. ================= " << endl << endl;

  return 0;
}
