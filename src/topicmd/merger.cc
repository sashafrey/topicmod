#include "merger.h"

#include <algorithm>
#include <iostream>

namespace topicmd {

Merger::Merger(const std::shared_ptr<const Generation>& generation,
               int topics_count) :
    lock_(),
    token_topic_matrix_(lock_),
    generation_(generation)
{
  auto ttm = std::make_shared<TokenTopicMatrix>();
  generation_->InvokeOnEachPartition(
      [&](std::shared_ptr<const Partition> part) {
        auto tokens = part->get_tokens();
        for (auto iter = tokens.begin();
             iter != tokens.end(); iter++)
        {
          ttm->add_token(*iter);
        }
      });

  ttm->Initialize(topics_count);
  float* target = ttm->token_topics(0);
  for (int i = 0; i < ttm->tokens_count() * ttm->topics_count(); ++i) {
    target[i] = (float)rand() / (float)RAND_MAX;
  }

  token_topic_matrix_.set(ttm);
}


void Merger::MergeFromQueueAndUpdateMatrix(
    std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
    boost::mutex& merger_queue_lock) {
  auto cur_ttm = token_topic_matrix_.get();
  auto new_ttm = std::make_shared<TokenTopicMatrix>();
  new_ttm->Initialize(*cur_ttm);
  float* target = new_ttm->token_topics(0);
  int token_size = cur_ttm->tokens_count();
  int topic_size = cur_ttm->topics_count();
  std::vector<float> topics(topic_size, 0);
  
  for (;;) {
    std::shared_ptr<const ProcessorOutput> processor_output;
    {
      boost::lock_guard<boost::mutex> guard(merger_queue_lock);
      if (merger_queue.empty()) {
        break;
      }

      processor_output = merger_queue.front();
      merger_queue.pop();
    }

    const float* source = processor_output->counter_token_topic(0);
    for (int i = 0; i < token_size * topic_size; i++) {
      target[i] += source[i];
    }

    for (int i = 0; i < topic_size; i++) {
      topics[i] += processor_output->counter_topic()[i];
    }
  }

  // divide phase
  for (int iToken = 0; iToken < token_size; ++iToken) {
    for (int iTopic = 0; iTopic < topic_size; ++iTopic) {
      target[iToken * topic_size + iTopic] /= topics[iTopic];
    }
  }

  token_topic_matrix_.set(new_ttm);

  // void logTopWordsPerTopic(const WordTopicMatrix& mat, int N) {
  {
    int wordsToSort = 7;
    for (int i = 0; i < topic_size; i++) {
      std::cout << "#" << (i+1) << ": ";
      std::vector<std::pair<float, int> > p_w;
      for (int iWord = 0; iWord < token_size; iWord++) {
        p_w.push_back(std::pair<float, int>(
            new_ttm->token_topics(iWord)[i], iWord));
      }

      std::sort(p_w.begin(), p_w.end());
      for (int iWord = token_size - 1;
           (iWord >= 0) && (iWord >= token_size - wordsToSort);
           iWord--)
      {
        std::cout << new_ttm->token(p_w[iWord].second) << " ";
      }

      std::cout << std::endl;
    }
  }
}


} // namespace topicmd