#ifndef TOPICMD_MERGER_
#define TOPICMD_MERGER_

#include "assert.h"
#include <stdlib.h>

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include <boost/thread.hpp>   
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include "topicmd/instance_schema.h"
#include "topicmd/internals.pb.h"
#include "topicmd/thread_safe_holder.h"
#include "topicmd/token_topic_matrix.h"

namespace artm { namespace core {

class Merger : boost::noncopyable {
 public:
  Merger(boost::mutex& merger_queue_lock,
         std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue,
         ThreadSafeHolder<InstanceSchema>& schema);

  ~Merger();

  void DisposeModel(int model_id);
  void UpdateModel(int model_id, const ModelConfig& model);

  std::shared_ptr<const TokenTopicMatrix> GetLatestTokenTopicMatrix(int model_id) const;
  
 private:
  mutable boost::mutex lock_;
  ThreadSafeCollectionHolder<int, TokenTopicMatrix> token_topic_matrix_;
  ThreadSafeHolder<InstanceSchema>& schema_;
  
  boost::mutex& merger_queue_lock_;
  std::queue<std::shared_ptr<const ProcessorOutput> >& merger_queue_; 

  boost::thread thread_;  
  void ThreadFunction();
};

}} // namespace artm/core

#endif // TOPICMD_MERGER_
