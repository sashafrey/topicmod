// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_HELPERS_H_
#define SRC_ARTM_CORE_HELPERS_H_

namespace artm {
namespace core {

class Helpers {
 public:
  // Usage: SetThreadName (-1, "MainThread");
  // (thread_id == -1 stands for the current thread)
  static void SetThreadName(int thread_id, const char* thread_name);
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_HELPERS_H_
