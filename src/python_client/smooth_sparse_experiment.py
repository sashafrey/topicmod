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
topics_count = 20
outer_iteration_count = 51
inner_iterations_count = 10
top_tokens_count_to_visualize = 10
parse_collection_from_text = 0

#######
critical_prob_mass = 0.5
no_kernel_topics = 2
background_topics_count = 3;

eps = 1e-8
perplexity_file = open('perplexity.txt', 'w')
top_words_file = open('top_words.txt', 'w')
theta_sparsity_file = open('theta_sparsity.txt', 'w')
phi_sparsity_file = open('phi_sparsity.txt', 'w')
#kernel_param_file = open('kernel_param.txt', 'w')
#kernel_words_file = open('kernel_words.txt', 'w')

#const = 0.7e+6

#reg_decor_coef = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#                  const, const, const, const, const, const, const, const, const, const,
#                  const, const, const, const, const, const, const, const, const, const,
#                  const, const, const, const, const, const, const, const, const, const,
#                  const, const, const, const, const, const, const, const, const, const]

#reg_theta_coef = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
#                  1.5, 1.7, 1.9, 2, 2.1, 2.3, 2.5, 2.7, 2.9, 3.1,
#                  3.3, 3.5, 3.7, 3.9, 4, 4.2, 4.4, 4.6, 4.8, 5,
#                  5.2, 5.4, 5.6, 5.8, 6, 6.2, 6.4, 6.6, 6.8, 7,
#                  7.2, 7.4, 7.6, 7.8, 8, 8.2, 8.4, 8.6, 8.8, 9]

#reg_phi_coef = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
#                1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
#                1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
#                2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
#                2, 2, 2, 2, 2, 2, 2, 2, 2, 2]

etalon_perplexity = [12176, 2471, 2276, 2021, 1848, 1746, 1684, 1644, 1617, 1597, 
                     1584, 1573, 1565, 1558, 1552, 1548, 1543, 1540, 1537, 1535, 
                     1533, 1530, 1528, 1527, 1525, 1524, 1522, 1521, 1520, 1520, 
                     1519, 1518, 1517, 1517, 1516, 1515, 1514, 1514, 1513, 1512, 
                     1512, 1511, 1511, 1510, 1510, 1509, 1508, 1507, 1507, 1506]
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
            alpha_ref.value.append(+0.08)

    reg_theta_name = 'regularizer_theta'
    reg_theta_type = 2
    regularizer_theta = master_component.CreateRegularizer(
        reg_theta_name, reg_theta_type, reg_theta_config)
    

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
    #######

    model_config = messages_pb2.ModelConfig()
    model_config.topics_count = topics_count
    model_config.inner_iterations_count = inner_iterations_count
    score_ = model_config.score.add()
    score_.type = Score_Type_Perplexity
 
    model = master_component.CreateModel(model_config)
    for iter in range(0, outer_iteration_count):
        master_component.InvokeIteration(1)
        master_component.WaitIdle();
        topic_model = master_component.GetTopicModel(model)

        #######
        if (iter == 0):
            for token in range(0, len(topic_model.token)):
                entry = dic_phi_config.entry.add()
                entry.key_token = topic_model.token[token]
                entry.value = 0
                for t in range(0, background_topics_count):
                    entry.values.value.append(+0.1)

            dictionary = master_component.CreateDictionary(dic_phi_config)

            #model_config.regularizer_name.append(reg_theta_name)
            #model_config.regularizer_tau.append(0)

            #model_config.regularizer_name.append(reg_phi_name)
            #model_config.regularizer_tau.append(0)

            #model_config.regularizer_name.append(reg_decor_name)
            #model_config.regularizer_tau.append(0)

            model.Reconfigure(model_config)

        else:
          
            if (iter == 9):
                dic_phi_config_new = messages_pb2.DictionaryConfig()
                dic_phi_config_new.name = dic_phi_name
                for token in range(0, len(topic_model.token)):
                    entry = dic_phi_config_new.entry.add()
                    entry.key_token = topic_model.token[token]
                    entry.value = -1
                    for t in range(0, background_topics_count):
                        entry.values.value.append(+0.1)

                dictionary.Reconfigure(dic_phi_config_new)

                reg_theta_config_new = messages_pb2.SmoothSparseThetaConfig()
                reg_theta_config_new.background_topics_count = background_topics_count
                for i in range(0, inner_iterations_count):
                # make case for different iterations here if need
                    alpha_ref = reg_theta_config_new.alpha.add()
                    for j in range(0, topics_count - background_topics_count):
                        alpha_ref.value.append(-1)
                    for j in range(0, background_topics_count):
                        alpha_ref.value.append(+0.08)

                regularizer_theta.Reconfigure(reg_theta_type, reg_theta_config_new)

            #i = FindName(model_config.regularizer_name, reg_theta_name)
            #model_config.regularizer_tau[i] = reg_theta_coef[iter - 1]

            #i = FindName(model_config.regularizer_name, reg_phi_name)
            #model_config.regularizer_tau[i] = reg_phi_coef[iter - 1]

            #i = FindName(model_config.regularizer_name, reg_decor_name)
            #model_config.regularizer_tau[i] = reg_decor_coef[iter - 1]

            model.Reconfigure(model_config)
            print model_config.regularizer_name
            print model_config.regularizer_tau
        
        #######

        if (iter > 0):
            model.InvokePhiRegularizers();
            print "Iter# = " + str(iter) + \
                    ", Items# = " + str(topic_model.items_processed) + \
                    ", Perplexity = " + str(topic_model.scores.value[0])
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

            zero_part = no_zero_elem / (topics_size * tokens_size * 1.0)
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
                zero_part = no_zero_elem / (background_topics_count * tokens_size * 1.0)
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
                                         
            zero_part = no_zero_elem / (topics_size * docs_size * 1.0)
            print '-----'
            print 'percentage of zero elements in Theta: ' + str(zero_part * 100.0) + ' %'
            theta_sparsity_file.write(str(zero_part * 100.0) + '\n')

            print '-----'
            print 'Real perplexity - etalon: ' + str(topic_model.scores.value[0] - 
                                                     etalon_perplexity[iter - 1])

            perplexity_file.write(str(topic_model.scores.value[0]) + '\n')







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
        print best_tokens.rstrip(', ')
        
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
