import operator

class Analyzer:
  @staticmethod
  def PrintTopTokensPerTopic(topic_model, how_many_tokens_to_visualize = 5):
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
      for best_token in range(0, how_many_tokens_to_visualize):
          best_tokens = best_tokens + sorted_token_map[best_token][0] + ', '
      print best_tokens

  @staticmethod
  def PrintThetaMatrix(theta_matrix, topics_size, how_many_items_to_visualize = 5):
    print "\nThetaMatrix (first " + str(how_many_items_to_visualize) + " documents):"
    for j in range(0, topics_size):
      print "Topic" + str(j) + ": ",
      for i in range(0, min(how_many_items_to_visualize, len(theta_matrix.item_id))):
        weight = theta_matrix.item_weights[i].value[j]
        print "%.3f\t" % weight,
      print "\n",
