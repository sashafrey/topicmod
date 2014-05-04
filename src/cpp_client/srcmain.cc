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
#include "glog/logging.h"

using namespace artm;

double proc(int argc, char * argv[], int processors_count, int instance_size) {
  // Create memcached server
  MemcachedServer memcached_server("tcp://*:5555");

  // Create instance and data_loader configs
  InstanceConfig instance_config;
  instance_config.set_processors_count(processors_count);
  instance_config.set_memcached_endpoint("tcp://localhost:5555");
  DataLoaderConfig data_loader_config;
  std::string batches_disk_path("batches");
  data_loader_config.set_disk_path(batches_disk_path);

  std::vector<std::shared_ptr<Instance>> instance;
  std::vector<std::shared_ptr<DataLoader>> data_loader;
  for (int i = 0; i < instance_size; ++i) {
    instance.push_back(std::make_shared<Instance>(instance_config));
    data_loader.push_back(std::make_shared<DataLoader>(*instance[i], data_loader_config));
  }

  // Configure train and test streams
  Stream train_stream, test_stream;
  train_stream.set_name("train_stream");
  train_stream.set_type(Stream_Type_ItemIdModulus);
  train_stream.set_modulus(10);
  for (int i = 0; i <= 8; ++i) {
    train_stream.add_residuals(i);
  }

  test_stream.set_name("test_stream");
  test_stream.set_type(Stream_Type_ItemIdModulus);
  test_stream.set_modulus(10);
  test_stream.add_residuals(9);

  for (int i = 0; i < instance_size; ++i) {
    data_loader[i]->AddStream(train_stream);
    data_loader[i]->AddStream(test_stream);
  }

  // Create model
  int nTopics = atoi(argv[3]);
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
  model_config.set_inner_iterations_count(10);
  model_config.set_stream_name("train_stream");
  model_config.set_reuse_theta(true);
  model_config.set_model_id("15081980-90a7-4767-ab85-7cb551c39339");

  Score* score = model_config.add_score();
  score->set_type(Score_Type_Perplexity);
  score->set_stream_name("test_stream");

  // todo(alfrey): model should be decoupled from instance
  std::vector<std::shared_ptr<Model>> model;
  for (int i = 0; i < instance_size; ++i) {
    model.push_back(std::make_shared<Model>(*instance[i], model_config));
  }

  int batch_files_count = countFilesInDirectory(batches_disk_path, ".batch");
  if (batch_files_count == 0) {
    std::cout << "No batches found, parsing collection from text files... ";
    // Load doc-word matrix
    DocWordMatrix::Ptr doc_word_ptr = loadMatrixFileUCI(argv[1]);
    VocabPtr vocab_ptr = loadVocab(argv[2]); //, doc_word_ptr->getW());
    int no_words = vocab_ptr->size(); //doc_word_ptr->getW();
    int no_docs = doc_word_ptr->getD();

    //int no_parts = 16;
    //int doc_index = 0;
    //int no_docs_per_part = no_docs / no_parts + 1;
    int no_docs_per_part = 1000;
    int no_parts = no_docs / no_docs_per_part + 1;
    int doc_index = 0;
    for (int part_index = 1; part_index <= no_parts; part_index++)
    {
      Batch batch;
      for (int i = 0; i < no_words; i++) {
        batch.add_token((*vocab_ptr)[i]);
      }

      for (; doc_index < (no_docs_per_part * part_index) && (doc_index < no_docs); doc_index++) {
        auto term_ids = doc_word_ptr->getTermId(doc_index);
        auto term_counts = doc_word_ptr->getFreq(doc_index);

        Item* item = batch.add_item();
        item->set_id(doc_index);
        Field* field = item->add_field();
        for (int word_index = 0; word_index < (int)term_ids.size(); ++word_index) {
          field->add_token_id(term_ids[word_index]);
          field->add_token_count((google::protobuf::int32) term_counts[word_index]);
        }
      }
      // Index doc-word matrix
      data_loader[part_index % instance_size]->AddBatch(batch);
    }

    std::cout << "OK.\n";
  } else {
    std::cout << "Found " << batch_files_count << " batches in folder '"
              << batches_disk_path << "', will use them.\n";
  }


  clock_t begin = clock();

  // Enable model and wait while each document pass through processor about 10 times.
  for (int i = 0; i < instance_size; ++i) model[i]->Enable();
  std::shared_ptr<TopicModel> topic_model;

  for (int iter = 0; iter < 10; ++iter) {
    for (int i = 0; i < instance_size; ++i) data_loader[i]->InvokeIteration(1);
    for (int i = 0; i < instance_size; ++i) data_loader[i]->WaitIdle();

    for (int inst = 0; inst < instance_size; ++inst) {
      topic_model = instance[inst]->GetTopicModel(*model[inst]);
      std::cout << "Iter #" << (iter + 1) << ": "
                << "Inst #" << (inst + 1) << ": "
                << "#Tokens = "  << topic_model->token_size() << ", "
                << "#Items = " << topic_model->items_processed() << ", "
                << "Perplexity = " << topic_model->scores().value(0) << endl;
    }
  }

  for (int i = 0; i < instance_size; ++i) model[i]->Disable();

  std::cout << endl;

  clock_t end = clock();

  // Log top 7 words per each topic
  {
    int wordsToSort = 7;
    int no_tokens = topic_model->token_size();
    int nTopics = topic_model->topics_count();

    for (int topic_index = 0; topic_index < nTopics; topic_index++) {
      std::cout << "#" << (topic_index+1) << ": ";

      std::vector<std::pair<float, std::string> > p_w;
      for (int token_index = 0; token_index < no_tokens; ++token_index) {
        string token = topic_model->token(token_index);
        float weight = topic_model->token_weights(token_index).value(topic_index);
          p_w.push_back(std::pair<float, std::string>(weight, token));
      }

        std::sort(p_w.begin(), p_w.end());
        for (int word_index = (int)p_w.size() - 1;
             (word_index >= 0) && (word_index >= (int)p_w.size() - wordsToSort);
             word_index--)
        {
          std::cout << p_w[word_index].second << " ";
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

  FLAGS_log_dir = ".";
  FLAGS_logbufsecs = 0;
  ::google::InitGoogleLogging(argv[0]);

  int instance_size = 2;
  int processors_size = 2;
  cout << proc(argc, argv, processors_size, instance_size)
       << " sec. ================= " << endl << endl;

  return 0;
}
