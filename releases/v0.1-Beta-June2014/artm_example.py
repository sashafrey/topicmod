import sys
sys.path.append('BigARTM')

from analyzer import *
from collection import *
from python_interface import *
import glob

os.environ['PATH'] = ';'.join([os.path.abspath(os.curdir) + '\\BigARTM', os.environ['PATH']])
library = ArtmLibrary(os.path.abspath(os.curdir) + '\\BigARTM\\artm.dll')

with library.CreateMasterComponent() as master:
  # Setup basic configuration
  master_config = master.config()
  master_config.processors_count = 2
  master_config.cache_theta = 1
  master.Reconfigure(master_config)

  # Configure a perplexity score calculator
  master.CreateScore(
    'perplexity_score',
    ScoreConfig_Type_Perplexity,
    messages_pb2.PerplexityScore())

  # Configure basic regularizers
  master.CreateRegularizer(
    'reg_theta',
    RegularizerConfig_Type_DirichletTheta,
    messages_pb2.DirichletThetaConfig())

  master.CreateRegularizer(
    'reg_phi',
    RegularizerConfig_Type_DirichletPhi,
    messages_pb2.DirichletPhiConfig())

  master.CreateRegularizer(
    'reg_decorrelator',
    RegularizerConfig_Type_DecorrelatorPhi,
    messages_pb2.DecorrelatorPhiConfig())

  # Configure the model
  model_config = messages_pb2.ModelConfig()
  model_config.topics_count = 4
  model_config.inner_iterations_count = 10
  model_config.score_name.append("perplexity_score")
  model_config.regularizer_name.append('reg_theta')
  model_config.regularizer_tau.append(0.1)
  model_config.regularizer_name.append('reg_phi')
  model_config.regularizer_tau.append(-0.1)
  model_config.regularizer_name.append('reg_decorrelator')
  model_config.regularizer_tau.append(10000)
  model = master.CreateModel(model_config)

  collection = Collection('vocab.kos.txt', 'docword.kos.txt', 1000)
  for batch in collection.batches:
    master.AddBatch(batch)

  for iter in range(1, 8):
    master.InvokeIteration(1)        # Invoke one scan of the entire collection...
    master.WaitIdle();               # and wait until it completes.
    model.InvokePhiRegularizers();
    topic_model = master.GetTopicModel(model)  # Retrieve topic model
    perplexity_score = master.GetScore(model, 'perplexity_score')

    print "Iter# = " + str(iter) + \
          ", Perplexity = " + str(perplexity_score.value)

  Analyzer.PrintTopTokensPerTopic(topic_model)
  Analyzer.PrintThetaMatrix(master.GetThetaMatrix(model), model_config.topics_count)
