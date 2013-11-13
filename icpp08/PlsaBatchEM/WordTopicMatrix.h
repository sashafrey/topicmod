#ifndef __WORD_TOPIC_MATRIX
#define __WORD_TOPIC_MATRIX

#include <assert.h>
#include <stdlib.h>

#include "Common.h"
#include "Vocab.h"

class WordTopicMatrix 
{
private:
    DISALLOW_COPY_AND_ASSIGN(WordTopicMatrix);
    int sizeW;
    int sizeT;
    DataType* data;
    const Vocab* vocab;
public:
    WordTopicMatrix(int _sizeW, int _sizeT, const Vocab* _vocab = NULL) : sizeW(_sizeW), sizeT(_sizeT), vocab(_vocab) 
    {
        assert(_sizeW > 0);
        assert(_sizeT > 0);
        if (_vocab != NULL) assert(_vocab->size() == _sizeW);

        // set up data and testData.
        data = new DataType[ sizeW * sizeT ];
    }

    ~WordTopicMatrix() {
        delete [] data;
    }

    const DataType* get() const {
        return data;
    }

    DataType* get() {
        return data;
    }

    int getW() const {
        return sizeW;
    }

    int getT() const {
        return sizeT;
    }

    const Vocab& getVocab() const {
        assert(vocab != NULL);
        return *vocab;
    }

    typedef std::auto_ptr<WordTopicMatrix> Ptr;
};


#endif // __WORD_TOPIC_MATRIX

