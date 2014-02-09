#include <ctime>
#include <cstring>
#include <iostream>

using namespace std;

#include "doc_token_matrix.h"
#include "token_topic_matrix.h"
#include "doc_topic_matrix.h"
#include "vocab.h"
#include "helpers.h"

#include "artm/cpp_interface.h"
#include "artm/messages.pb.h"

using namespace artm;

double proc(int argc, char * argv[], int processors_count) {
  // Create instance
  InstanceConfig instance_config;
  instance_config.set_processors_count(processors_count);
  Instance instance(instance_config);

  // Create data loader
  DataLoaderConfig data_loader_config;
  DataLoader data_loader(instance, data_loader_config);

  // Create model
  int nTopics = atoi(argv[3]);
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
  model_config.set_inner_iterations_count(10);
  Model model(instance, model_config);
  
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
    data_loader.AddBatch(batch);
  }

  clock_t begin = clock();

  // Enable model and wait while each document pass through processor about 10 times.
  model.Enable();
  instance.WaitModelProcessed(model, nDocs * 10);
  model.Disable();

  clock_t end = clock();

  std::shared_ptr<ModelTopics> model_topics = instance.GetTopics(model);

  std::cout << "Number of tokens in model topic: " 
            << model_topics->token_topic_size()
            << endl;

  // Log top 7 words per each topic
  {
    int wordsToSort = 7;
    int nTokens = model_topics->token_topic_size();
    int nTopics = model_topics->token_topic(0).topic_weight_size();

    for (int iTopic = 0; iTopic < nTopics; iTopic++) {
      std::cout << "#" << (iTopic+1) << ": ";

      std::vector<std::pair<float, std::string> > p_w;
      for (int iToken = 0; iToken < nTokens; ++iToken) {
        const TokenTopics& token_topic = model_topics->token_topic(iToken);
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
