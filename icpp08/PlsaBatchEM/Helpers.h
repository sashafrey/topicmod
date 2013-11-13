#ifndef __HELPERS
#define __HELPERS

#include <memory>
#include <string>

#include "DocWordMatrix.h"
#include "WordTopicMatrix.h"
#include "Vocab.h"

DocWordMatrix::Ptr loadMatrixFileUCI(std::string matFileName);
VocabPtr loadVocab(std::string vocabFile, int nWords);
void logTopWordsPerTopic(const WordTopicMatrix& mat, int N);

#endif // __HELPERS_H