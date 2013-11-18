#include <cstring>
#include <iostream>

using namespace std;

#include "DocWordMatrix.h"
#include "WordTopicMatrix.h"
#include "DocTopicMatrix.h"
#include "Vocab.h"
#include "Helpers.h"

int main(int argc, char * argv[]) {
    if (argc != 4) {
        cout << "Usage: ./PlsaBatchEM <docword> <vocab> nTopics" << endl;
        return 0;
    }

    DocWordMatrix::Ptr pD = loadMatrixFileUCI(argv[1]);
    VocabPtr pVocab = loadVocab(argv[2], pD->getW());
    int nTopics = atoi(argv[3]);

    int nWords = pD->getW();
    int nDocs = pD->getD();
    WordTopicMatrix phi(nWords, nTopics, pVocab.get());
    for (int i = 0; i < nWords * nTopics; ++i) {
        phi.get()[i] = (DataType)rand() / (DataType)RAND_MAX;
    }           

    for (int iOuterIter = 0; iOuterIter <= 10; ++iOuterIter) {
        WordTopicMatrix hat_n_wt(nWords, nTopics);
        memset(&hat_n_wt.get()[0], 0, nWords * nTopics * sizeof(DataType));

        std::vector<DataType> hat_n_t(nTopics);
        memset(&hat_n_t[0], 0, nTopics * sizeof(DataType));

        for (int iDoc = 0; iDoc < nDocs; ++iDoc) 
        {
            vector<int>& thisDoc_wordIds = pD->getTermId(iDoc);
            vector<DataType>& thisDoc_wordFreqs = pD->getFreq(iDoc);
            int thisDoc_wordsCount = thisDoc_wordIds.size();

            std::vector<DataType> theta_t(nTopics);
            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                theta_t[iTopic] = (DataType)rand() / (DataType)RAND_MAX;
            }           

            std::vector<DataType> Z(thisDoc_wordsCount);
            int numInnerIters = 10;
            for (int iInnerIter = 0; iInnerIter <= numInnerIters; iInnerIter++) {
                // 1. Find Z
                for (int thisDoc_iWord = 0; thisDoc_iWord < thisDoc_wordsCount; ++thisDoc_iWord) {
                    DataType curZ((DataType)0);
                    int iWord = thisDoc_wordIds[thisDoc_iWord]; // global wordId
                    DataType* wordFreq = phi.get() + iWord * nTopics;
                    for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                        curZ += wordFreq[iTopic] * theta_t[iTopic];
                    }

                    Z[thisDoc_iWord] = curZ;
                }

                // 2. Find new theta (or store results if on the last iteration)
                std::vector<DataType> theta_t_next(nTopics);
                memset(&theta_t_next[0], 0, nTopics * sizeof(DataType));
                for (int thisDoc_iWord = 0; thisDoc_iWord < thisDoc_wordsCount; ++thisDoc_iWord) {
                    DataType n_dw = thisDoc_wordFreqs[thisDoc_iWord];
                    int iWord = thisDoc_wordIds[thisDoc_iWord]; // global wordId
                    DataType* wordFreq = phi.get() + iWord * nTopics;
                    DataType curZ = Z[thisDoc_iWord];
                    if (curZ > 0) {
                        if (iInnerIter < numInnerIters) {
                            // Normal iteration, updating theta_t_next
                            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                                theta_t_next[iTopic] += n_dw * wordFreq[iTopic] * theta_t[iTopic] / curZ;
                            }
                        } else {
                            // Last iteration, updating final counters
                            DataType* hat_n_wt_cur = hat_n_wt.get() + iWord * nTopics;
                            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                                 DataType val = n_dw * wordFreq[iTopic] * theta_t[iTopic] / curZ;
                                 hat_n_wt_cur[iTopic] += val;
                                 hat_n_t[iTopic] += val;
                            }
                        }
                    }
                }

                if (iInnerIter < numInnerIters) {
                    for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                        theta_t[iTopic] = theta_t_next[iTopic];
                    }                
                }
            }
        }

        for (int iWord = 0; iWord < nWords; ++iWord) {
            for (int iTopic = 0; iTopic < nTopics; ++iTopic) {
                phi.get()[iWord * nTopics + iTopic] = hat_n_wt.get()[iWord * nTopics + iTopic] / hat_n_t[iTopic];
            }
        }

        logTopWordsPerTopic(phi, 7);
        cout << endl;
    }

    return 0;
}
