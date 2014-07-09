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

from __future__ import division
import sys
import os
import time

if sys.platform.count('linux') == 1:
    interface_address = os.path.abspath(os.path.join(os.curdir, os.pardir, 'python_interface'))
    sys.path.append(interface_address)
else:
    sys.path.append('../python_interface/')

import messages_pb2
from python_interface import *
import operator
import random

#######
def FindName(list_name, name):
    for i in range(0, len(list_name)):
        if (list_name[i] == name):
            return i
    return -1

def FindSeveralMax(list, no_max):
    max_idx = []
    max_val = []
    
    for cur_max in range(0, no_max):  
        max_idx.append(-1)
        max_val.append(-1)
        for i in range(0, len(list)):
            if not(i in max_idx):
                if (list[i] > max_val[cur_max]):
                    max_idx[cur_max] = i
                    max_val[cur_max] = list[i]
    return [max_val, max_idx]
#######

with open('../../datasets/vocab.nips.txt', 'r') as content_file:
    content = content_file.read()

tokens = content.split('\n')

# Some configuration numbers
batch_size = 500
processors_count = 2
limit_collection_size = 50000 # don't load more that this docs
topics_count = 100
outer_iteration_count = 41
inner_iterations_count = 1
top_tokens_count_to_visualize = 10
parse_collection_from_text = 1

#######
#critical_prob_mass = 0.5
#no_kernel_topics = 2
background_topics_count = 10;

eps = 1e-100
perplexity_file = open('perplexity.txt', 'w')
top_words_file = open('top_words.txt', 'w')
theta_sparsity_file = open('theta_sparsity.txt', 'w')
phi_sparsity_file = open('phi_sparsity.txt', 'w')
top10_file = open('top10.txt', 'w')
top100_file = open('top100.txt', 'w')

#kernel_param_file = open('kernel_param.txt', 'w')
#kernel_words_file = open('kernel_words.txt', 'w')

decor = False
sp_phi = False
sm_phi = False
sp_theta = False
sm_theta = False

const = 200000
not_need = 0

reg_decor_coef = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                  const * 0.1, const * 0.2, const * 0.3, const * 0.4, const * 0.5, 
                  const * 0.6, const * 0.7, const * 0.8, const * 0.9, const,
                  const, const, const, const, const,
                  const, const, const, const, const,
                  const, const, const, const, const,
                  const, const, const, const, const, not_need]

reg_theta_coef = [0, 0, 0, 0, 0, 0, 0, 0, 0,
                  60, 80, 100, 120, 140, 160, 180, 200, 220, 240,
                  260, 280, 300, 320, 340, 360, 380, 400, 420, 430,
                  440, 450, 460, 460, 460, 460, 470, 470, 470, 470, not_need]

reg_theta_smooth_coef = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, not_need]

reg_phi_coef = [0, 0, 0, 0, 0, 0, 0, 0, 0, 
                0.003, 0.007, 0.008, 0.009, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 
                0.011, 0.012, 0.013, 0.014, 0.015, 0.016, 0.017, 0.018, 0.019, 0.02, 
                0.021, 0.022, 0.024, 0.026, 0.028, 0.030, 0.034, 0.038, 0.042, 0.046, not_need]

reg_phi_smooth_coef = [2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                       2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                       2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                       2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                       2, 2, 2, 2, 2, 2, 2, 2, 2, 2, not_need]

etalon_perplexity = [12350, 2567, 2565, 2561, 2556, 2549, 2537, 2523, 2503, 2476, 
                      2444, 2403, 2355, 2301, 2243, 2184, 2123, 2073, 2021, 1978,
                      1937, 1898, 1866, 1832, 1805, 1778, 1754, 1736, 1718, 1694, 
                      1681, 1670, 1651, 1640, 1631, 1620, 1611, 1600, 1597, 1584]
#######

address = os.path.abspath(os.path.join(os.curdir, os.pardir))

if sys.platform.count('linux') == 1:
    library = ArtmLibrary(address + '/bin/libartm.so')
else:
    os.environ['PATH'] = ';'.join([address + '\\Win32\\Release', os.environ['PATH']])
    library = ArtmLibrary(address + '\\Win32\\Release\\artm.dll')

master_config = messages_pb2.MasterComponentConfig()
master_config.processors_count = processors_count
master_config.cache_processor_output = 1
master_config.disk_path = '.'

perplexity_config = messages_pb2.PerplexityScoreConfig();
score_config = master_config.score_config.add()
score_config.config = messages_pb2.PerplexityScoreConfig().SerializeToString();
score_config.type = ScoreConfig_Type_Perplexity;
score_config.name = "perplexity_score"

with library.CreateMasterComponent(master_config) as master_component:
    batch = messages_pb2.Batch()
    batch_tokens = {}
    prev_item_id = -1

    if (parse_collection_from_text):
        with open('../../datasets/docword.nips.txt', 'r') as docword:
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
                        master_component.AddBatch(batch)
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
            master_component.AddBatch(batch)

    #######
    reg_decor_config = messages_pb2.DecorrelatorPhiConfig()
    reg_decor_name = 'regularizer_decor'
    reg_decor_type = 4
    regularizer_decor = master_component.CreateRegularizer(
        reg_decor_name, reg_decor_type, reg_decor_config)

    reg_theta_config = messages_pb2.SmoothSparseThetaConfig()
    reg_theta_config.background_topics_count = background_topics_count
    for i in range(0, inner_iterations_count):
    # make case for different iterations here if need
        alpha_ref = reg_theta_config.alpha.add()
        for j in range(0, topics_count - background_topics_count):
            alpha_ref.value.append(0)
        for j in range(0, background_topics_count):
            alpha_ref.value.append(0)

    reg_theta_name = 'regularizer_theta'
    reg_theta_type = 2
    regularizer_theta = master_component.CreateRegularizer(
        reg_theta_name, reg_theta_type, reg_theta_config)
    

    reg_theta_smooth_config = messages_pb2.SmoothSparseThetaConfig()
    reg_theta_smooth_config.background_topics_count = background_topics_count
    for i in range(0, inner_iterations_count):
    # make case for different iterations here if need
        if (i == 0):
            alpha_ref = reg_theta_smooth_config.alpha.add()
            for j in range(0, topics_count - background_topics_count):
                alpha_ref.value.append(0)
            for j in range(0, background_topics_count):
                alpha_ref.value.append(+0.05)
        elif (0 < i <= 3):
            alpha_ref = reg_theta_smooth_config.alpha.add()
            for j in range(0, topics_count - background_topics_count):
                alpha_ref.value.append(0)
            for j in range(0, background_topics_count):
                alpha_ref.value.append(+0.2)   
        elif (3 < i <= 6):
            alpha_ref = reg_theta_smooth_config.alpha.add()
            for j in range(0, topics_count - background_topics_count):
                alpha_ref.value.append(0)
            for j in range(0, background_topics_count):
                alpha_ref.value.append(+0.5)                            
        elif (6 < i <= 9):
            alpha_ref = reg_theta_smooth_config.alpha.add()
            for j in range(0, topics_count - background_topics_count):
                alpha_ref.value.append(0)
            for j in range(0, background_topics_count):
                alpha_ref.value.append(+1) 

    reg_theta_smooth_name = 'regularizer_theta_smooth'
    reg_theta_smooth_type = 2
    regularizer_theta_smooth = master_component.CreateRegularizer(
        reg_theta_smooth_name, reg_theta_smooth_type, reg_theta_smooth_config)


    reg_phi_config = messages_pb2.SmoothSparsePhiConfig()
    reg_phi_config.background_topics_count = background_topics_count
    
    dic_phi_name = 'dictionary_phi'
    dic_phi_config = messages_pb2.DictionaryConfig()
    dic_phi_config.name = dic_phi_name
    
    reg_phi_config.dictionary_name = dic_phi_name
    reg_phi_name = 'regularizer_phi'
    reg_phi_type = 3
    regularizer_phi = master_component.CreateRegularizer(
        reg_phi_name, reg_phi_type, reg_phi_config)


    reg_phi_smooth_config = messages_pb2.SmoothSparsePhiConfig()
    reg_phi_smooth_config.background_topics_count = background_topics_count
    
    dic_phi_smooth_name = 'dictionary_phi_smooth'
    dic_phi_smooth_config = messages_pb2.DictionaryConfig()
    dic_phi_smooth_config.name = dic_phi_smooth_name
    
    reg_phi_smooth_config.dictionary_name = dic_phi_smooth_name
    reg_phi_smooth_name = 'regularizer_phi_smooth'
    reg_phi_smooth_type = 3
    regularizer_phi_smooth = master_component.CreateRegularizer(
        reg_phi_smooth_name, reg_phi_smooth_type, reg_phi_smooth_config)

    #######

    model_config = messages_pb2.ModelConfig()
    model_config.topics_count = topics_count
    model_config.inner_iterations_count = inner_iterations_count
    model_config.score_name.append('perplexity_score')
 
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
        start = time.clock()
        master_component.InvokeIteration(1)
        master_component.WaitIdle();
        elapsed = (time.clock() - start)
        if (iter > 0):
            print 'Elapsed time for iteration: ' + str(elapsed)
        topic_model = master_component.GetTopicModel(model)
        perplexity_score = master_component.GetScore(model, 'perplexity_score')

        #######
        if (iter == 0):
            for token in range(0, len(topic_model.token)):
                entry = dic_phi_config.entry.add()
                entry.key_token = topic_model.token[token]
                entry.value = 0
                for t in range(0, background_topics_count):
                    entry.values.value.append(0)

            dictionary = master_component.CreateDictionary(dic_phi_config)

            for token in range(0, len(topic_model.token)):
                entry = dic_phi_smooth_config.entry.add()
                entry.key_token = topic_model.token[token]
                entry.value = 0
                for t in range(0, background_topics_count):
                    entry.values.value.append(+0.05)

            dictionary_smooth = master_component.CreateDictionary(dic_phi_smooth_config)

            if (sp_theta == True):
                model_config.regularizer_name.append(reg_theta_name)
                model_config.regularizer_tau.append(0)

            if (sm_theta == True):
                model_config.regularizer_name.append(reg_theta_smooth_name)
                model_config.regularizer_tau.append(0)

            if (sp_phi == True):
                model_config.regularizer_name.append(reg_phi_name)
                model_config.regularizer_tau.append(0)

            if (sm_phi == True):
                model_config.regularizer_name.append(reg_phi_smooth_name)
                model_config.regularizer_tau.append(0)

            if (decor == True):
                model_config.regularizer_name.append(reg_decor_name)
                model_config.regularizer_tau.append(0)

            model.Reconfigure(model_config)

        else:

            if (iter == 10):
                dic_phi_config_new = messages_pb2.DictionaryConfig()
                dic_phi_config_new.name = dic_phi_name
                for token in range(0, len(topic_model.token)):
                    entry = dic_phi_config_new.entry.add()
                    entry.key_token = topic_model.token[token]
                    entry.value = -1
                    for t in range(0, background_topics_count):
                        entry.values.value.append(0)

                dictionary.Reconfigure(dic_phi_config_new)

                if (sp_theta == True):
                    reg_theta_config_new = messages_pb2.SmoothSparseThetaConfig()
                    reg_theta_config_new.background_topics_count = background_topics_count
                    for i in range(0, inner_iterations_count):
                    # make case for different iterations here if need
                        if (i == 0):
                            alpha_ref = reg_theta_config_new.alpha.add()
                            for j in range(0, topics_count - background_topics_count):
                                alpha_ref.value.append(-0.05)
                            for j in range(0, background_topics_count):
                                alpha_ref.value.append(0)
                        elif (0 < i <= 3):
                            alpha_ref = reg_theta_config_new.alpha.add()
                            for j in range(0, topics_count - background_topics_count):
                                alpha_ref.value.append(-0.2)
                            for j in range(0, background_topics_count):
                                alpha_ref.value.append(0)   
                        elif (3 < i <= 6):
                            alpha_ref = reg_theta_config_new.alpha.add()
                            for j in range(0, topics_count - background_topics_count):
                                alpha_ref.value.append(-0.5)
                            for j in range(0, background_topics_count):
                                alpha_ref.value.append(0)                            
                        elif (6 < i <= 9):
                            alpha_ref = reg_theta_config_new.alpha.add()
                            for j in range(0, topics_count - background_topics_count):
                                alpha_ref.value.append(-1)
                            for j in range(0, background_topics_count):
                                alpha_ref.value.append(0) 

                    regularizer_theta.Reconfigure(reg_theta_type, reg_theta_config_new)


            if (sp_theta == True):
                i = FindName(model_config.regularizer_name, reg_theta_name)
                model_config.regularizer_tau[i] = reg_theta_coef[iter - 1]

            if (sm_theta == True):
                i = FindName(model_config.regularizer_name, reg_theta_smooth_name)
                model_config.regularizer_tau[i] = reg_theta_smooth_coef[iter - 1]

            if (sp_phi == True):
                i = FindName(model_config.regularizer_name, reg_phi_name)
                model_config.regularizer_tau[i] = reg_phi_coef[iter - 1]

            if (sm_phi == True):
                i = FindName(model_config.regularizer_name, reg_phi_smooth_name)
                model_config.regularizer_tau[i] = reg_phi_smooth_coef[iter - 1]

            if (decor == True):
                i = FindName(model_config.regularizer_name, reg_decor_name)
                model_config.regularizer_tau[i] = reg_decor_coef[iter - 1]

            model.Reconfigure(model_config)
            print model_config.regularizer_name
            print model_config.regularizer_tau

            #tm_internals = messages_pb2.TopicModel.TopicModelInternals()
            #tm_internals.ParseFromString(topic_model.internals)
            #for token_index in range(0, 100):
            #    s = ''
            #    for topic_index in range(14, 20):
            #        add_s = str(round(tm_internals.n_wt[token_index].value[topic_index], 1))
            #        s = s + add_s.rjust(8)
            #    print s
            #print '~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~'
        ######

        if (iter > 0):
            start = time.clock()
            model.InvokePhiRegularizers();
            elapsed = (time.clock() - start)
            print 'Elapsed time for Phi regularization: ' + str(elapsed)
            print '---------'
            print "Iter# = " + str(iter) + \
                    ", Perplexity = " + str(perplexity_score.value)
        else:
            print 'start!\n'

        #######
        if (iter > 0):
            no_zero_elem = 0;
            tokens_size = len(topic_model.token)
            topics_size = topic_model.topics_count - background_topics_count
            topics_count = topic_model.topics_count

            for token_index in range(0, tokens_size):
                for topic_index in range(0, topics_size):
                    token_weight = topic_model.token_weights[token_index].value[topic_index]
                    if (abs(token_weight) < eps):
                        no_zero_elem = no_zero_elem + 1

            zero_part = no_zero_elem / (topics_size * tokens_size)
            print '-----'
            print 'percentage of zero elements in Phi: ' + str(zero_part * 100.0) + ' %'
            phi_sparsity_file.write(str(zero_part * 100.0) + '\n')

            no_zero_elem = 0
            if (background_topics_count > 0):
                for topic_index in range(topics_size, topic_model.topics_count):
                    for token_index in range(0, tokens_size):
                        token_weight = topic_model.token_weights[token_index].value[topic_index]
                        if (abs(token_weight) < eps):
                            no_zero_elem = no_zero_elem + 1
                zero_part = no_zero_elem / (background_topics_count * tokens_size)
                print '-----'
                print 'percentage of zero elements in Phi (bcgd): ' + str(zero_part * 100.0) + ' %'


            no_zero_elem = 0    
            theta_matrix = master_component.GetThetaMatrix(model)
            docs_size = len(theta_matrix.item_id)

            for j in range(0, topics_size):
                for i in range(0, docs_size):
                    weight = theta_matrix.item_weights[i].value[j]
                    if (abs(weight) < eps):
                        no_zero_elem = no_zero_elem + 1                  
                                         
            zero_part = no_zero_elem / (topics_size * docs_size)
            print '-----'
            print 'percentage of zero elements in Theta: ' + str(zero_part * 100.0) + ' %'
            theta_sparsity_file.write(str(zero_part * 100.0) + '\n')

            print '-----'
            print 'Real perplexity - etalon: ' + str(perplexity_score.value - 
                                                     etalon_perplexity[iter - 1])

            perplexity_file.write(str(perplexity_score.value) + '\n')

            min_top_size = 10
            max_top_size = 100

            token_map = {}
            min_top_tokens = []
            max_top_tokens = []

            for topic_index in range(0, topics_size):
                for token_index in range(0, tokens_size):
                    token = topic_model.token[token_index];
                    token_weight = topic_model.token_weights[token_index].value[topic_index]
                    token_map[token] = token_weight
                sorted_token_map = sorted(token_map.iteritems(), key=operator.itemgetter(1), reverse=True)
                for best_token in range(0, min_top_size):
                    min_top_tokens.append(sorted_token_map[best_token][0])

                for best_token in range(0, max_top_size):
                    max_top_tokens.append(sorted_token_map[best_token][0])
            
            min_top_tokens_set = set(min_top_tokens)    
            unique_tokens_size = len(min_top_tokens_set)
            all_tokens_size = len(min_top_tokens)

            print '-----'
            value = unique_tokens_size / (1.0 * all_tokens_size)
            print 'percentage of unique tokens in Top10: ' + str(value * 100.0) + ' %'

            top10_file.write(str(str(value * 100.0)) + '\n')

            max_top_tokens_set = set(max_top_tokens)
            unique_tokens_size = len(max_top_tokens_set)
            all_tokens_size = len(max_top_tokens)
            
            print '-----'
            value = unique_tokens_size / (1.0 * all_tokens_size)
            print 'percentage of unique tokens in Top100: ' + str(value * 100.0) + ' %'

            top100_file.write(str(str(value * 100.0)) + '\n')

            #tm_internals = messages_pb2.TopicModel.TopicModelInternals()
            #tm_internals.ParseFromString(topic_model.internals)
            #for token_index in range(0, 100):
            #    s = ''
            #    for topic_index in range(14, 20):
            #        add_s = str(round(tm_internals.n_wt[token_index].value[topic_index], 0))
            #        s = s + add_s + '|   '
            #    print s

            #p_tw = []
            #tm_internals = messages_pb2.TopicModel.TopicModelInternals()
            #tm_internals.ParseFromString(topic_model.internals)
            #for token_index in range(0, tokens_size):
            #    p_tw.append([])
            #    norm = 0
            #    for topic_index in range(0, topics_count):
            #        n_wt = tm_internals.n_wt[token_index].value[topic_index]
            #        r_wt = tm_internals.r_wt[token_index].value[topic_index]
            #        p_tw[token_index].append(n_wt + r_wt)
            #        norm = norm + n_wt + r_wt
            #    if (norm > eps):
            #        for topic_index in range(0, topics_count):
            #            p_tw[token_index][topic_index] = p_tw[token_index][topic_index] / (
            #                1.0 * norm)




            #topics_kernel = []
            #for topic_index in range(0, topics_size):
            #    topics_kernel.append([])

            #kernel_size = 0
            #kernel_purity = 0

            #for token_index in range(0, tokens_size):
            #    [max_values, max_indices] = FindSeveralMax(p_tw[token_index], no_kernel_topics)
            #    if (sum(max_values) > critical_prob_mass):
            #        for i in max_indices:
            #            if (i < topics_size):
            #                topics_kernel[i].append(token_index)
            #                kernel_size = kernel_size + 1
            #                phi_wt = topic_model.token_weights[token_index].value[i]
            #                kernel_purity = kernel_purity + phi_wt

            #kernel_size = kernel_size / (1.0 * topics_size)
            #kernel_purity = kernel_purity / (1.0 * topics_size)

            #kernel_contrasty = 0
            #for topic_index in range(0, topics_size):
            #    topic_kernel_size = len(topics_kernel[topic_index])
            #    if (topic_kernel_size > 0):
            #        p_tw_sum = 0
            #        for i in topics_kernel[topic_index]:
            #            p_tw_sum = p_tw_sum + p_tw[i][topic_index]
            #        kernel_contrasty = kernel_contrasty + p_tw_sum / (1.0 * topic_kernel_size)

            #kernel_contrasty = kernel_contrasty / (1.0 * topics_size)
                
            #print '-----'
            
            #ker_str = str(kernel_size) + ' | ' + str(kernel_purity) + ' | ' + str(kernel_contrasty)
            #print 'Kernel size | purity | contrasty : ' + ker_str

            #kernel_param_file.write(ker_str + '\n')

            #if (iter == (outer_iteration_count - 1)):
            #    for i in range(0, topics_size):
            #        for j in range(0, len(topics_kernel[i])):
            #            kernel_words_file.write(topic_model.token[topics_kernel[i][j]] + ', ')
            #        kernel_words_file.write('\n --- \n')

            print '==============================================='
        #######

    # Log to top words in each topic
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
        #print best_tokens.rstrip(', ')
        
        #######
        top_words_file.write(best_tokens.rstrip(', ') + '\n')
        #######
    
    docs_to_show = 9
    print "\nThetaMatrix (first " + str(docs_to_show) + " documents):"
    theta_matrix = master_component.GetThetaMatrix(model)
    for j in range(0, topics_size):
      print "Topic" + str(j) + ": ",
      for i in range(0, min(docs_to_show, len(theta_matrix.item_id))):
        weight = theta_matrix.item_weights[i].value[j]
        print "%.3f\t" % weight,
      print "\n",

    print 'Finish!'
