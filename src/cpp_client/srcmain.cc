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

int main(int argc, char * argv[]) {
  if (argc != 4) {
    cout << "Usage: ./PlsaBatchEM <docword> <vocab> nTopics" << endl;
    return 0;
  }

  // Create instance
  InstanceConfig instance_config;
  string instance_config_blob;
  instance_config.SerializeToString(&instance_config_blob);
  int instance_id = 
    create_instance(
      0,
      instance_config_blob.size(), 
      string_as_array(&instance_config_blob));

  // Create model
  int nTopics = atoi(argv[3]);
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
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

  Batch batch;
  for (int i = 0; i < nWords; i++) {
    batch.add_token((*pVocab)[i]);
  }

  for (int iDoc = 0; iDoc < nDocs; iDoc++) {
    auto term_ids = pD->getTermId(iDoc);
    auto term_counts = pD->getFreq(iDoc);

    Item* item = batch.add_item();
    Field* field = item->add_field();
    for (int iWord = 0; iWord < term_ids.size(); ++iWord) {
      field->add_token_id(term_ids[iWord]);
      field->add_token_count(term_counts[iWord]);
    }
  }

  // Index doc-word matrix
  string batch_blob;
  batch.SerializeToString(&batch_blob);
  insert_batch(instance_id, batch_blob.size(), string_as_array(&batch_blob));
  // insert_batch(instance_id, batch);
  int generation_id = finish_partition(instance_id);
  publish_generation(instance_id, generation_id);

  wait_model_processed(instance_id, model_id, nDocs * 10);

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

  cout << "Number of tokens in model topic: " 
       << model_topics.token_topic_size()
       << endl;

  // dispose_request(request_id);
  dispose_model(instance_id, model_id);
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
        for (int iWord = p_w.size() - 1;
             (iWord >= 0) && (iWord >= p_w.size() - wordsToSort);
             iWord--)
        {
          std::cout << p_w[iWord].second << " ";
        }
      
      std::cout << std::endl;
    }
  }

  return 0;
}
