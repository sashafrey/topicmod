import sys
import os

if sys.platform.count('linux') == 1:
    interface_address = os.path.abspath(os.path.join(os.curdir, os.pardir, 'python_interface'))
    sys.path.append(interface_address)
else:
    sys.path.append('../python_interface/')

import messages_pb2
from python_interface import *
import operator
import random
import glob


# Some configuration numbers
batch_size = 500
processors_count = 4
eps = 1e-100
limit_collection_size = 50000 # don't load more that this docs
topics_count = 20
outer_iteration_count = 10
inner_iterations_count = 10
top_tokens_count_to_visualize = 4

vocab_file = '../../datasets/vocab.kos.txt'
docword_file = '../../datasets/docword.kos.txt'

address = os.path.abspath(os.path.join(os.curdir, os.pardir))

if sys.platform.count('linux') == 1:
    library = ArtmLibrary(address + '/bin/libartm.so')
else:
    os.environ['PATH'] = ';'.join([address + '\\Win32\\Release', os.environ['PATH']])
    library = ArtmLibrary(address + '\\Win32\\Release\\artm.dll')


with open(vocab_file, 'r') as content_file:
    content = content_file.read()

tokens = content.splitlines()

master_config = messages_pb2.MasterComponentConfig()
master_config.processors_count = processors_count
master_config.cache_theta = 1
master_config.disk_path = '.'

perplexity_config = messages_pb2.PerplexityScoreConfig();
score_config = master_config.score_config.add()
score_config.config = messages_pb2.PerplexityScoreConfig().SerializeToString();
score_config.type = ScoreConfig_Type_Perplexity;
perplexity_score_name = "perplexity_score"
score_config.name = perplexity_score_name

sparsity_theta_config = messages_pb2.SparsityThetaScoreConfig();
score_config = master_config.score_config.add()
score_config.config = messages_pb2.SparsityThetaScoreConfig().SerializeToString();
score_config.type = ScoreConfig_Type_SparsityTheta;
sparsity_theta_score_name = "sparsity_theta_score"
score_config.name = sparsity_theta_score_name

sparsity_phi_config = messages_pb2.SparsityPhiScoreConfig();
score_config = master_config.score_config.add()
score_config.config = messages_pb2.SparsityPhiScoreConfig().SerializeToString();
score_config.type = ScoreConfig_Type_SparsityPhi;
sparsity_phi_score_name = "sparsity_phi_score"
score_config.name = sparsity_phi_score_name

batches_found = len(glob.glob("*.batch"))
if batches_found == 0:
    print "No batches found, parsing them from textual collection ",
    batch = messages_pb2.Batch()
    batch_tokens = {}
    prev_item_id = -1

    with open('../../datasets/docword.kos.txt', 'r') as docword:
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
                    print ".",
                    library.SaveBatch(batch, master_config.disk_path)
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
        library.SaveBatch(batch, master_config.disk_path)
    print " OK."
else:
    print "Found " + str(batches_found) + " batches, using them."

with library.CreateMasterComponent(master_config) as master_component:
    model_config = messages_pb2.ModelConfig()
    model_config.topics_count = topics_count
    model_config.inner_iterations_count = inner_iterations_count
    model_config.score_name.append('perplexity_score')

    ################################################################################
    regularizer_config_theta = messages_pb2.DirichletThetaConfig()
    regularizer_name_theta = 'regularizer_theta'
    regularizer_theta = master_component.CreateRegularizer(
      regularizer_name_theta,
      RegularizerConfig_Type_DirichletTheta,
      regularizer_config_theta)

    regularizer_config_decor = messages_pb2.DecorrelatorPhiConfig()
#     regularizer_decor_config.background_topics_count = background_topics_count
    regularizer_name_decor = 'regularizer_decor'
    regularizer_decor = master_component.CreateRegularizer(
      regularizer_name_decor,
      RegularizerConfig_Type_DecorrelatorPhi,
      regularizer_config_decor)

    ################################################################################
    model_config = messages_pb2.ModelConfig()
    model_config.topics_count = topics_count
    model_config.inner_iterations_count = inner_iterations_count

    model_config.score_name.append(perplexity_score_name)
    model_config.score_name.append(sparsity_theta_score_name)
    model_config.score_name.append(sparsity_phi_score_name)

#     model_config.regularizer_name.append(regularizer_name_theta)
#     model_config.regularizer_tau.append(0.1)
#     model_config.regularizer_name.append(regularizer_name_decor)
#     model_config.regularizer_tau.append(200000)

    model = master_component.CreateModel(model_config)
    initial_topic_model = messages_pb2.TopicModel();
    initial_topic_model.topics_count = topics_count;
    initial_topic_model.name = model.name()

    random.seed(123)
    for token in tokens:
        initial_topic_model.token.append(token);
        weights = initial_topic_model.token_weights.add();
        for topic_index in range(0, topics_count):
            weights.value.append(random.random())
    model.Overwrite(initial_topic_model)

    for iter in range(0, outer_iteration_count):
        master_component.InvokeIteration(1)
        master_component.WaitIdle(120000);
        topic_model = master_component.GetTopicModel(model)
        perplexity_score = master_component.GetScore(model, perplexity_score_name)
        sparsity_theta_score = master_component.GetScore(model, sparsity_theta_score_name)
        sparsity_phi_score = master_component.GetScore(model, sparsity_phi_score_name)

        model.InvokePhiRegularizers();

        print "Iter# = " + str(iter) + \
                ", Perplexity = " + str(perplexity_score.value) + \
                ", SparsityTheta = " + str(sparsity_theta_score.value) +\
                ", SparsityPhi = " + str(sparsity_phi_score.value)


    # Log to 7 words in each topic
    tokens_size = len(topic_model.token)
    topics_size = topic_model.topics_count

    for topic_index in range(0, topics_size):
        token_map = {}
        best_tokens = '#' + str(topic_index + 1) + ': '
        for token_index in range(0, tokens_size):
            token = topic_model.token[token_index];
            token_weight = topic_model.token_weights[token_index].value[topic_index]
            token_map[token] = token_weight
        sorted_token_map = sorted(token_map.iteritems(), key=operator.itemgetter(1), reverse=True)
        for best_token in range(0, top_tokens_count_to_visualize):
            best_tokens = best_tokens + sorted_token_map[best_token][0] + ', '
        print best_tokens.rstrip(', ')

    docs_to_show = 7
    print "\nThetaMatrix (first " + str(docs_to_show) + " documents):"
    theta_matrix = master_component.GetThetaMatrix(model)
    for j in range(0, topics_size):
        print "Topic" + str(j) + ": ",
        for i in range(0, min(docs_to_show, len(theta_matrix.item_id))):
            weight = theta_matrix.item_weights[i].value[j]
            print "%.3f\t" % weight,
        print "\n",

    print 'Done with regularization!'
