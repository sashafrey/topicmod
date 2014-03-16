// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_HELPERS_H_
#define SRC_ARTM_HELPERS_H_

namespace artm {
namespace core {

// Usage: SetThreadName (-1, "MainThread");
// (thread_id == -1 stands for the current thread)
void SetThreadName(int thread_id, char* thread_name);

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_HELPERS_H_
