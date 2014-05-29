import sys
sys.path.append('../python_interface/')

import messages_pb2
import python_interface
from python_interface import *
import operator

with open('..\\..\\datasets\\vocab.kos.txt', 'r') as content_file:
    content = content_file.read()

tokens = content.split('\n')

# Some configuration numbers
batch_size = 500
processors_count = 4
limit_collection_size = 50000  # don't load more that this docs
topics_count = 16
outer_iteration_count = 10
inner_iterations_count = 10
top_tokens_count_to_visualize = 4
parse_collection_from_text = 0

address = os.path.abspath(os.path.join(os.curdir, os.pardir))
os.environ['PATH'] = ';'.join([address + '\\Win32\\Release', os.environ['PATH']])
library = ArtmLibrary(address + '\\Win32\\Release\\artm.dll')

master_config = messages_pb2.MasterComponentConfig()
master_config.processors_count = processors_count
master_config.disk_path = '.'
with library.CreateMasterComponent(master_config) as master_component:
    batch = messages_pb2.Batch()
    batch_tokens = {}
    prev_item_id = -1

    if (parse_collection_from_text):
        with open('..\\..\\datasets\\docword.kos.txt', 'r') as docword:
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

    model_config = messages_pb2.ModelConfig()
    model_config.topics_count = topics_count
    model_config.inner_iterations_count = inner_iterations_count
    score_ = model_config.score.add()
    score_.type = 0

    ################################################################################
    # include in model one Dirihlet Theta regularizer

    regularizer_config_theta = messages_pb2.DirichletThetaConfig()
    for i in range(0, inner_iterations_count):
        # make case for different iterations here if need
        regularizer_config_theta.alpha_0.append(-1)
        tilde_alpha_ref = regularizer_config_theta.tilde_alpha.add()
        for j in range(0, topics_count):
            tilde_alpha_ref.value.append(0.1)

    regularizer_name_theta = 'regularizer_theta'
    model_config.regularizer_name.append(regularizer_name_theta)
    regularizer_theta = master_component.CreateRegularizer(regularizer_name_theta, 0, regularizer_config_theta)

    ################################################################################
    # include in model one Dirihlet Phi regularizer

    regularizer_config_phi = messages_pb2.DirichletPhiConfig()
    regularizer_config_phi.beta_0 = -0.01
    for j in range(0, 2):
        regularizer_config_phi.tilde_beta.value.append(0.1)

    regularizer_name_phi = 'regularizer_phi'
    model_config.regularizer_name.append(regularizer_name_phi)
    regularizer_phi = master_component.CreateRegularizer(regularizer_name_phi, 1, regularizer_config_phi)
    ################################################################################

    model = master_component.CreateModel(model_config)
    model.Enable()
    for iter in range(0, outer_iteration_count):
        master_component.InvokeIteration(1)
        master_component.WaitIdle();
        topic_model = master_component.GetTopicModel(model)

        ################################################################################
        # include in model one Dirihlet Phi regularizer

        regularizer_config_phi = messages_pb2.DirichletPhiConfig()
        regularizer_config_phi.beta_0 = -0.01
        token_size = len(topic_model.token)
        for j in range(0, token_size):
            regularizer_config_phi.tilde_beta.value.append(0.1)

        regularizer_phi.Reconfigure(1, regularizer_config_phi)
        ################################################################################
        model.InvokePhiRegularizers();

        print "Iter# = " + str(iter) + \
                ", Items# = " + str(topic_model.items_processed) + \
                ", Perplexity = " + str(topic_model.scores.value[0])
    model.Disable();

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
        print best_tokens

    print 'Done with regularization!'
