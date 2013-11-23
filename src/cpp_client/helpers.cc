#include "helpers.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "doc_topic_matrix.h"
#include "token_topic_matrix.h"
#include "vocab.h"

using namespace std;

DocWordMatrix::Ptr loadMatrixFileUCI(string matFileName)
{
    ifstream matFile( matFileName.c_str() );

    if ( !matFile.good() )
    {
        matFile.close();
        throw "Error opening matrix file.";
    }

    int sizeD, sizeW, numNonZero;
    matFile >> sizeD >> sizeW >> numNonZero;

    DocWordMatrix::Ptr retval(new DocWordMatrix(sizeD, sizeW));
    
    int docId, termId, freq;
    for (int i = 0; i < numNonZero; i++) 
    {
        matFile >> docId >> termId >> freq;
        
        // convert unity-based indexes into zero-based
        docId--; 
        termId--;
        retval->getFreq(docId).push_back( (DataType)freq );
        retval->getTermId(docId).push_back( termId );
    }

    matFile.close();
    return retval;
}

VocabPtr loadVocab(string vocabFile, int nWords) {
  ifstream matFile( vocabFile.c_str() );
    if ( !matFile.good() )
    {
        matFile.close();
        throw "Error opening dictionary file.";
    }

    VocabPtr vocab(new Vocab());
    char buff[128];
    for (int iWord = 0; iWord < nWords; ++iWord) {
        matFile.getline(buff, 128);
        vocab->push_back(buff);
    }

    return vocab;
}

void logTopWordsPerTopic(const WordTopicMatrix& mat, int N) {
    int wordsToSort = N;
    int nTopics = mat.getT();
    int nWords = mat.getW();
    for (int i = 0; i < nTopics; i++) {
        cout << "#" << (i+1) << ": ";
        std::vector<std::pair<DataType, int> > p_w;
        for (int iWord = 0; iWord < mat.getW(); iWord++) {
            p_w.push_back(std::pair<DataType, int>(mat.get()[iWord * nTopics + i], iWord));
        }

        std::sort(p_w.begin(), p_w.end());
        for (int iWord = nWords - 1; (iWord >= 0) && (iWord >= nWords - wordsToSort); iWord--) {
            cout << mat.getVocab()[p_w[iWord].second] << " ";
        }

        cout << endl;
    }

}
