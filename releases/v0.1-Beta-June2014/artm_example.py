import sys
sys.path.append('BigARTM')

from analyzer import *
from collection import *
from python_interface import *
import glob

os.environ['PATH'] = ';'.join([os.path.abspath(os.curdir) + '\\BigARTM', os.environ['PATH']])
library = ArtmLibrary(os.path.abspath(os.curdir) + '\\BigARTM\\artm.dll')

# Setup basic configuration
master_config = messages_pb2.MasterComponentConfig()
master_config.processors_count = 2        # Use two concurrent workers
master_config.cache_processor_output = 1  # Set this option if later you want to GetThetaMatrix()
master_config.disk_path = '.'             # Disk location to store/load batches
with library.CreateMasterComponent(master_config) as master:
  # On the first run parse batches and pass them to master.AddBatch().
  # The library serializes all batches to disk as '.batch' files.
  # On second run the library will automatically load all '.batch' files from
  # the location, specified by "master_config.disk_path"
  if len(glob.glob("*.batch")) == 0:
    collection = Collection('vocab.kos.txt', 'docword.kos.txt', 1000)
    for batch in collection.batches:
      master.AddBatch(batch)

  # Configure train stream (90% of the items) and test stream (10%)
  train_stream = messages_pb2.Stream()
  test_stream = messages_pb2.Stream()
  train_stream.name = "train_stream"
  train_stream.type = Stream_Type_ItemIdModulus
  train_stream.modulus = 10
  for i in range(0, 8):
    train_stream.residuals.append(i)

  test_stream.name = "test_stream"
  test_stream.type = Stream_Type_ItemIdModulus
  test_stream.modulus = 10
  test_stream.residuals.append(9)

  master.AddStream(train_stream);
  master.AddStream(test_stream);

  # Configure the model
  model_config = messages_pb2.ModelConfig()
  model_config.topics_count = 16               # Create a topics model with 4 topics
  model_config.inner_iterations_count = 10     # Perform 10 iterations on inner loop (BatchPLSA algorithm)
  model_config.stream_name = "train_stream"    # Only use training data to tune the model
  score = model_config.score.add()             # Calculate one score (perplexity)
  score.type = Score_Type_Perplexity
  score.stream_name = "test_stream"            # Use testing data to calculate perplexity

  master.CreateRegularizer(
    'regularizer_theta',
    RegularizerConfig_Type_DirichletTheta,
    messages_pb2.DirichletThetaConfig())

  master.CreateRegularizer(
    'regularizer_phi',
    RegularizerConfig_Type_DirichletPhi,
    messages_pb2.DirichletPhiConfig())

  model_config.regularizer_name.append('regularizer_theta')
  model_config.regularizer_tau.append(0.1)
  model_config.regularizer_name.append('regularizer_phi')
  model_config.regularizer_tau.append(-0.1)

  model = master.CreateModel(model_config)
  for iter in range(1, 10):
    master.InvokeIteration(1)        # Invoke one scan of the entire collection...
    master.WaitIdle();               # and wait until it completes.
    model.InvokePhiRegularizers();
    topic_model = master.GetTopicModel(model)  # Retrieve topic model

    print "Iter# = " + str(iter) + \
          ", Items# = " + str(topic_model.items_processed) + \
          ", Perplexity = " + str(topic_model.scores.value[0])

  Analyzer.PrintTopTokensPerTopic(topic_model)
  Analyzer.PrintThetaMatrix(master.GetThetaMatrix(model), model_config.topics_count)
