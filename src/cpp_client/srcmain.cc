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
  int instance_id = create_instance(0,
    instance_config_blob.size(), 
  	string_as_array(&instance_config_blob));
  // int instance_id = create_instance(0, instance_config);

  // Create model
  int nTopics = atoi(argv[3]);
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
   string model_config_blob;
   model_config.SerializeToString(&model_config_blob);
   int model_id = create_model(instance_id, 
   			      0,
  			      model_config_blob.size(), 
  			      string_as_array(&model_config_blob));
  // int model_id = create_model(instance_id, 0, model_config);
  
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

  cout << "Number of documents: " << batch.item().size();
  for (int i = 0; i < batch.item().size(); i++) {
    cout << batch.item().Get(i).field().Get(0).token_id().size() << " ";
  }

  // Index doc-word matrix
  string batch_blob;
  batch.SerializeToString(&batch_blob);
  insert_batch(instance_id, batch_blob.size(), string_as_array(&batch_blob));
  // insert_batch(instance_id, batch);
  int generation_id = finish_partition(instance_id);
  publish_generation(instance_id, generation_id);

	// todo: add sleep 50 ms.

  // Request model topics
  int length;
  char* address;
  int request_id = request_model_topics(instance_id, 
  				model_id, 
  				&length,
  				&address);

  string model_topics_blob;
  model_topics_blob.resize(length);
  copy_request_result(request_id, 
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

  /*
    WordTopicMatrix phi(nWords, nTopics, pVocab.get());
    for (int i = 0; i < nWords * nTopics; ++i) {
        phi.get()[i] = (DataType)rand() / (DataType)RAND_MAX;
    }           

    for (int iOuterIter = 0; iOuterIter <= 10; ++iOuterIter) {
        WordTopicMatrix hat_n_wt(nWords, nTopics);
        memset(&hat_n_wt.get()[0], 0, nWords * nTopics * sizeof(DataType));

        std::vector<DataType> hat_n_t(nTopics);
        memset(&hat_n_t[0], 0, nTopics * sizeof(DataType));

        for (int iDoc = 0; iDoc < nDocs; ++iDoc) 
        {
            vector<int>& thisDoc_wordIds = pD->getTermId(iDoc);
            vector<DataType>& thisDoc_wordFreqs = pD->getFreq(iDoc);
            int thisDoc_wordsCount = thisDoc_wordIds.size();

            std::vector<DataType> theta_t(nTopics);
            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                theta_t[iTopic] = (DataType)rand() / (DataType)RAND_MAX;
            }           

            std::vector<DataType> Z(thisDoc_wordsCount);
            int numInnerIters = 10;
            for (int iInnerIter = 0; iInnerIter <= numInnerIters; iInnerIter++) {
                // 1. Find Z
                for (int thisDoc_iWord = 0; thisDoc_iWord < thisDoc_wordsCount; ++thisDoc_iWord) {
                    DataType curZ((DataType)0);
                    int iWord = thisDoc_wordIds[thisDoc_iWord]; // global wordId
                    DataType* wordFreq = phi.get() + iWord * nTopics;
                    for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                        curZ += wordFreq[iTopic] * theta_t[iTopic];
                    }

                    Z[thisDoc_iWord] = curZ;
                }

                // 2. Find new theta (or store results if on the last iteration)
                std::vector<DataType> theta_t_next(nTopics);
                memset(&theta_t_next[0], 0, nTopics * sizeof(DataType));
                for (int thisDoc_iWord = 0; thisDoc_iWord < thisDoc_wordsCount; ++thisDoc_iWord) {
                    DataType n_dw = thisDoc_wordFreqs[thisDoc_iWord];
                    int iWord = thisDoc_wordIds[thisDoc_iWord]; // global wordId
                    DataType* wordFreq = phi.get() + iWord * nTopics;
                    DataType curZ = Z[thisDoc_iWord];
                    if (curZ > 0) {
                        if (iInnerIter < numInnerIters) {
                            // Normal iteration, updating theta_t_next
                            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                                theta_t_next[iTopic] += n_dw * wordFreq[iTopic] * theta_t[iTopic] / curZ;
                            }
                        } else {
                            // Last iteration, updating final counters
                            DataType* hat_n_wt_cur = hat_n_wt.get() + iWord * nTopics;
                            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                                 DataType val = n_dw * wordFreq[iTopic] * theta_t[iTopic] / curZ;
                                 hat_n_wt_cur[iTopic] += val;
                                 hat_n_t[iTopic] += val;
                            }
                        }
                    }
                }

                if (iInnerIter < numInnerIters) {
                    for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                        theta_t[iTopic] = theta_t_next[iTopic];
                    }                
                }
            }
        }

        for (int iWord = 0; iWord < nWords; ++iWord) {
            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                phi.get()[iWord * nTopics + iTopic] = hat_n_wt.get()[iWord * nTopics + iTopic] / hat_n_t[iTopic];
            }
        }

        logTopWordsPerTopic(phi, 7);
        cout << endl;
    }
  */
    return 0;
}
