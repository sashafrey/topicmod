import sys
import messages_pb2

class Collection:
  def __init__(self, vocab, docword, max_batch_size = 100):
    print("Loading data"),
    with open(vocab, 'r') as content_file:
      self.tokens = content_file.read().split('\n')

    self.batches = []
    batch = messages_pb2.Batch()
    for token in self.tokens:
      batch.token .append(token)

    with open('docword.kos.txt', 'r') as docword:
      items_count = int(docword.readline())
      words_count = int(docword.readline())
      num_non_zero = int(docword.readline())
      prev_item_id = -1

      for line in docword:
        item_id, token_id, frequency = [int(x) for x in line.split()]
        token = self.tokens[token_id - 1]

        if (item_id != prev_item_id):
          prev_item_id = item_id

          if (len(batch.item) >= max_batch_size):
            sys.stdout.write(".")
            self.batches.append(batch)
            batch = messages_pb2.Batch()
            for token in self.tokens:
              batch.token.append(token)

          item = batch.item.add()
          item.id = item_id
          field = item.field.add()

        field.token_id.append(token_id - 1)
        field.token_count.append(frequency)

    if (len(batch.item) > 0):
      self.batches.append(batch)

    print(' Done.')
