#include "helpers.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

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

  int sizeD, sizeW, num_non_zero;
  matFile >> sizeD >> sizeW >> num_non_zero;

  DocWordMatrix::Ptr retval(new DocWordMatrix(sizeD, sizeW));

  int docId, termId, freq;
  for (int i = 0; i < num_non_zero; i++)
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
  for (int word_index = 0; word_index < nWords; ++word_index) {
    matFile.getline(buff, 128);
    vocab->push_back(buff);
  }

  return vocab;
}

VocabPtr loadVocab(string vocabFileName) {
  ifstream vocabFile( vocabFileName.c_str() );
  if ( !vocabFile.good() )
  {
    vocabFile.close();
    throw "Error opening dictionary file.";
  }

  VocabPtr vocab(new Vocab());
  char buff[128];
  while(vocabFile.getline(buff, 128)) {
    vocab->push_back(buff);
  }

  return vocab;
}

void logTopWordsPerTopic(const WordTopicMatrix& mat, int N) {
  int words_to_sort = N;
  int no_topics = mat.getT();
  int no_words = mat.getW();
  for (int i = 0; i < no_topics; i++) {
    cout << "#" << (i+1) << ": ";
    std::vector<std::pair<DataType, int> > p_w;
    for (int word_index = 0; word_index < mat.getW(); word_index++) {
      p_w.push_back(std::pair<DataType, int>(mat.get()[word_index * no_topics + i], word_index));
    }

    std::sort(p_w.begin(), p_w.end());
    for (int word_index = no_words - 1; (word_index >= 0) && (word_index >= no_words - words_to_sort); word_index--) {
      cout << mat.getVocab()[p_w[word_index].second] << " ";
    }

    cout << endl;
  }
}

int countFilesInDirectory(std::string root, std::string ext) {
{
  int retval = 0;
  if (boost::filesystem::exists(root) && boost::filesystem::is_directory(root)) {
    boost::filesystem::recursive_directory_iterator it(root);
    boost::filesystem::recursive_directory_iterator endit;
    while(it != endit) {
      if (boost::filesystem::is_regular_file(*it) && it->path().extension() == ext) {
        retval++;
      }
      ++it;
    }
  }
  return retval;
}
}
