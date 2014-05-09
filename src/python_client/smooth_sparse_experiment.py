#################################################################################
# This file contains the source code of experiment with 
# Smooth/Sparse regularizer from BigARTM library. Used 
# dataset is NIPS. The results of experiments can be found 
# in corresponding part of documentation.
# 
# The experiment was prepared and proceed by 
# Murat Apishev (great-mel@yandex.ru) 
#
# The base of code is the BigARTM python_client.py
#################################################################################

import messages_pb2
import python_interface
from python_interface import *
import operator

with open('..\\..\\datasets\\vocab.nips.txt', 'r') as content_file:
  content = content_file.read()

tokens = content.split('\n')

# Some configuration numbers
batch_size = 500
processors_count = 2
limit_collection_size = 50000  # don't load more that this docs
topics_count = 20
background_topics_count = 3;
outer_iteration_count = 20
inner_iterations_count = 10
top_tokens_count_to_visualize = 7

address = os.path.abspath(os.path.join(os.curdir, os.pardir))
os.environ['PATH'] = ';'.join([address + '\\Win32\\Release', os.environ['PATH']])
library = ArtmLibrary(address + '\\Win32\\Release\\artm.dll')

instance_config = messages_pb2.InstanceConfig()
instance_config.processors_count = processors_count
with library.CreateInstance(instance_config) as instance:
    data_loader_config = messages_pb2.DataLoaderConfig()
    with library.CreateDataLoader(instance, data_loader_config) as data_loader:
        batch = messages_pb2.Batch()
        batch_tokens = {}
        prev_item_id = -1
        with open('..\\..\\datasets\\docword.nips.txt', 'r') as docword:
            items_count = int(docword.readline())
            words_count = int(docword.readline())
            num_non_zero = int(docword.readline())
            for line in docword:
                item_id, global_token_id, frequency = [int(x) for x in line.split()]
                token = tokens[global_token_id - 1]

                if (item_id != prev_item_id):
                    prev_item_id = item_id

                    if (item_id > limit_collection_size):
                        break

                    if (len(batch.item) >= batch_size):
                        data_loader.AddBatch(batch)
                        batch = messages_pb2.Batch()
                        batch_tokens = {}

                    item = batch.item.add()
                    item.id = item_id
                    field = item.field.add()

                if (not batch_tokens.has_key(token)):
                    batch_tokens[token] = len(batch.token)
                    batch.token.append(token)

                local_token_id = batch_tokens[token]
                field.token_id.append(local_token_id)
                field.token_count.append(frequency)

        if (len(batch.item) > 0):
            data_loader.AddBatch(batch)

        model_config = messages_pb2.ModelConfig()
        model_config.topics_count = topics_count
        model_config.inner_iterations_count = inner_iterations_count
        score_ = model_config.score.add()
        score_.type = 0

        ################################################################################
        # include in model one Smooth/Sparse Theta regularizer

        regularizer_config_theta = messages_pb2.SmoothSparseThetaConfig()
        for i in range(0, inner_iterations_count):
          # make case for different iterations here if need
          regularizer_config_theta.alpha_0.append(-1)
          tilde_alpha_ref = regularizer_config_theta.tilde_alpha.add()
          for j in range(0, topics_count):
            tilde_alpha_ref.value.append(0.1)

        regularizer_name_theta = 'regularizer_theta'
        general_regularizer_config_theta = messages_pb2.RegularizerConfig()
        general_regularizer_config_theta.name = regularizer_name_theta
        general_regularizer_config_theta.type = 2
        general_regularizer_config_theta.config = regularizer_config_theta.SerializeToString()

        model_config.regularizer_name.append(regularizer_name_theta)
        regularizer_theta = library.CreateRegularizer(instance, general_regularizer_config_theta)
        
        ################################################################################
        # include in model one Smooth/Sparse Phi regularizer

        regularizer_config_phi_init = messages_pb2.SmoothSparsePhiConfig()
        regularizer_config_phi_init.beta_0 = -0.01
        regularizer_config_phi_init.background_topics_count = background_topics_count
        for j in range(0, 2):
          regularizer_config_phi_init.tilde_beta.value.append(0.1)

        for i in range(0, background_topics_count):
          regularizer_config_phi_init.background_beta_0.append(-1)
          tilde_beta_ref = regularizer_config_phi_init.background_tilde_beta.add()
          for j in range(0, 2):
            tilde_beta_ref.value.append(0.1)        

        regularizer_name_phi_init = 'regularizer_phi'
        general_regularizer_config_phi_init = messages_pb2.RegularizerConfig()
        general_regularizer_config_phi_init.name = regularizer_name_phi_init
        general_regularizer_config_phi_init.type = 3
        general_regularizer_config_phi_init.config = regularizer_config_phi_init.SerializeToString()

        model_config.regularizer_name.append(regularizer_name_phi_init)
        regularizer_phi = library.CreateRegularizer(instance, general_regularizer_config_phi_init)

        ################################################################################

        with library.CreateModel(instance, model_config) as model:
            model.Enable()
            for iter in range(0, outer_iteration_count):
                data_loader.InvokeIteration(1)
                data_loader.WaitIdle();
                topics = instance.GetTopics(model)

                ################################################################################
                # include in model one Smooth/Sparse Phi regularizer

                regularizer_config_phi = messages_pb2.SmoothSparsePhiConfig()
                regularizer_config_phi.beta_0 = -0.01
                regularizer_config_phi.background_topics_count = background_topics_count
                token_size = len(topics.token_topic)
                for j in range(0, token_size):
                    regularizer_config_phi.tilde_beta.value.append(0.1)

                for i in range(0, background_topics_count):
                    regularizer_config_phi.background_beta_0.append(-1)
                    tilde_beta_ref = regularizer_config_phi.background_tilde_beta.add()
                    for j in range(0, token_size):
                        tilde_beta_ref.value.append(0.1)        

                regularizer_name_phi = 'regularizer_phi'
                general_regularizer_config_phi = messages_pb2.RegularizerConfig()
                general_regularizer_config_phi.name = regularizer_name_phi
                general_regularizer_config_phi.type = 3
                general_regularizer_config_phi.config = regularizer_config_phi.SerializeToString()
                
                regularizer_phi.Reconfigure(general_regularizer_config_phi)

                ################################################################################
                model.InvokePhiRegularizers();

                print "Iter# = " + str(iter) + \
                      ", Items# = " + str(topics.items_processed) + \
                      ", Perplexity = " + str(topics.score[0])
            model.Disable();

            # Log to 7 words in each topic
            tokens_size = len(topics.token_topic)
            topics_size = len(topics.token_topic[0].topic_weight)
            for topic_index in range(0, topics_size):
                token_map = {}
                best_tokens = '#' + str(topic_index + 1) + ': '
                for token_index in range(0, tokens_size):
                    token = topics.token_topic[token_index]
                    token_map[token.token]=token.topic_weight[topic_index]
                sorted_token_map = sorted(token_map.iteritems(), key=operator.itemgetter(1), reverse=True)
                for best_token in range(0, top_tokens_count_to_visualize):
                    best_tokens = best_tokens + sorted_token_map[best_token][0] + ', '
                print best_tokens

            print 'Done!'