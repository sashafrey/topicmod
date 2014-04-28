#ifndef __HELPERS
#define __HELPERS

#include <memory>
#include <string>

#include "doc_token_matrix.h"
#include "token_topic_matrix.h"
#include "vocab.h"

DocWordMatrix::Ptr loadMatrixFileUCI(std::string matFileName);
VocabPtr loadVocab(std::string vocabFile, int nWords);
VocabPtr loadVocab(std::string vocabFile);
void logTopWordsPerTopic(const WordTopicMatrix& mat, int N);

#endif // __HELPERS_H
