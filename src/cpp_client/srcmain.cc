#include <ctime>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <vector>

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
  std::string batches_disk_path = "batches";
  std::string vocab_file = "";
  std::string docword_file = "";

  bool is_network_mode = (instance_size > 1);

  MasterComponentConfig master_config;
  master_config.set_processors_count(processors_count);
  master_config.set_disk_path(batches_disk_path);
  if (is_network_mode) {
    master_config.set_modus_operandi(MasterComponentConfig_ModusOperandi_Network);
    master_config.set_master_component_create_endpoint("tcp://*:5555");
    master_config.set_master_component_connect_endpoint("tcp://localhost:5555");
  } else {
    master_config.set_modus_operandi(MasterComponentConfig_ModusOperandi_Local);
    master_config.set_cache_processor_output(true);
  }

  MasterComponent master_component(master_config);

  std::vector<std::shared_ptr<::artm::NodeController>> node_controller;
  if (is_network_mode) {
    for (int port = 5556; port < 5556 + instance_size; ++port) {
      ::artm::NodeControllerConfig node_config;
      node_config.set_master_component_connect_endpoint("tcp://localhost:5555");

      std::stringstream port_str;
      port_str << port;
      node_config.set_node_controller_create_endpoint(std::string("tcp://*:") + port_str.str());
      node_config.set_node_controller_connect_endpoint(std::string("tcp://localhost:") + port_str.str());
      node_controller.push_back(std::make_shared<::artm::NodeController>(node_config));
    }

    master_component.Reconfigure(master_config);  // Push configuration to clients
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

  master_component.AddStream(train_stream);
  master_component.AddStream(test_stream);

  RegularizerConfig regularizer_config;
  regularizer_config.set_name("regularizer_phi");
  regularizer_config.set_type(::artm::RegularizerConfig_Type_DirichletPhi);
  regularizer_config.set_config(::artm::DirichletPhiConfig().SerializeAsString());
  Regularizer dirichlet_phi_regularizer(master_component, regularizer_config);

  // Create model
  int nTopics = atoi(argv[3]);
  ModelConfig model_config;
  model_config.set_topics_count(nTopics);
  model_config.set_inner_iterations_count(10);
  model_config.set_stream_name("train_stream");
  model_config.set_reuse_theta(true);
  model_config.set_name("15081980-90a7-4767-ab85-7cb551c39339");
  model_config.add_regularizer_name("regularizer_phi");
  model_config.add_regularizer_tau(0.1);

  Score* score = model_config.add_score();
  score->set_type(Score_Type_Perplexity);
  score->set_stream_name("test_stream");
  Model model(master_component, model_config);

  std::cout << rand() << std::endl;
  // Overwrite topic model with well-known "initial topic model"
  // (skip this in network mode because the operation is not supported yet)
  TopicModel initial_topic_model;
  initial_topic_model.set_name(model_config.name());
  initial_topic_model.set_topics_count(nTopics);
  initial_topic_model.set_items_processed(0);
  initial_topic_model.mutable_scores()->add_value(0.0);  // add one dummy score
  VocabPtr vocab_ptr = loadVocab(vocab_file.empty() ? argv[2] : vocab_file);
  for (int token_index = 0; token_index < (int)vocab_ptr->size(); ++token_index) {
    std::string token = (*vocab_ptr)[token_index];
    initial_topic_model.add_token(token);
    artm::FloatArray* weights = initial_topic_model.add_token_weights();
    for (int topic_index = 0; topic_index < nTopics; ++topic_index) {
      weights->add_value((float) rand() / (float)RAND_MAX);
    }
  }

  model.Overwrite(initial_topic_model);

  int batch_files_count = countFilesInDirectory(batches_disk_path, ".batch");
  if (batch_files_count == 0) {
    std::cout << "No batches found, parsing collection from text files... ";
    // Load doc-word matrix
    auto doc_word_ptr = loadMatrixFileUCI(docword_file.empty() ? argv[1] : docword_file);
    VocabPtr vocab_ptr = loadVocab(vocab_file.empty() ? argv[2] : vocab_file);
    int no_words = vocab_ptr->size();
    int no_docs = doc_word_ptr->getD();

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
      master_component.AddBatch(batch);
    }

    std::cout << "OK.\n";
  } else {
    std::cout << "Found " << batch_files_count << " batches in folder '"
              << batches_disk_path << "', will use them.\n";
  }

  clock_t begin = clock();

  std::shared_ptr<TopicModel> topic_model;
  for (int iter = 0; iter < 10; ++iter) {
    master_component.InvokeIteration(1);
    master_component.WaitIdle();
    model.InvokePhiRegularizers();

    topic_model = master_component.GetTopicModel(model);
    std::cout << "Iter #" << (iter + 1) << ": "
              << "#Tokens = "  << topic_model->token_size() << ", "
              << "#Items = " << topic_model->items_processed() << ", "
              << "Perplexity = " << topic_model->scores().value(0) << endl;
  }

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

  if (!is_network_mode) {
    int docs_to_show = 7;
    std::cout << "\nThetaMatrix (first " << docs_to_show << " documents):\n";
    std::shared_ptr<ThetaMatrix> theta_matrix = master_component.GetThetaMatrix(model);
    for (int j = 0; j < nTopics; ++ j) {
      std::cout << "Topic" << j << ": ";
      for (int i = 0; i < min(docs_to_show, theta_matrix->item_id_size()); ++i) {
        float weight = theta_matrix->item_weights(i).value(j);
        std::cout << std::fixed << std::setw( 6 ) << std::setprecision( 3 ) << weight << "\t";
      }

      std::cout << "\n";
    }
  } else {
    // std::cout << "GetThetaMatrix is not implemented in Network modus operandi."; // todo(alfrey)
  }

  node_controller.clear();  // Destroy nodes; todo(alfrey): it should be OK to stop master before nodes.

  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  return elapsed_secs;
}

int main(int argc, char * argv[]) {
  if (argc != 4) {
    cout << "Usage: ./PlsaBatchEM <docword> <vocab> nTopics" << endl;
    return 0;
  }

  int instance_size = 1;
  int processors_size = 2;
  cout << proc(argc, argv, processors_size, instance_size)
       << " sec. ================= " << endl << endl;

  return 0;
}
