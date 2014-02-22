#ifndef __DOC_WORD_MATRIX
#define __DOC_WORD_MATRIX

#include <memory>
#include <vector>

#include <assert.h>

#include "common.h"

class DocWordMatrix
{
private:
    DISALLOW_COPY_AND_ASSIGN(DocWordMatrix);
    std::vector<DataType> *dataFreq;
    std::vector<int> *dataTermId;
    int sizeD;
    int sizeW;
public:
    DocWordMatrix(int _sizeD, int _sizeW) : sizeD(_sizeD), sizeW(_sizeW)
    {
        assert(_sizeD > 0);
        assert(_sizeW > 0);

        // set up data and testData.
        dataFreq = new std::vector<DataType>[ sizeD ];
        dataTermId = new std::vector<int>[ sizeD ];
    }

    std::vector<DataType>& getFreq(int docId)
    {
        assert(docId < sizeD && docId >= 0);
        return dataFreq[docId];
    }

    std::vector<int>& getTermId(int docId)
    {
        assert(docId < sizeD && docId >= 0);
        return dataTermId[docId];
    }

    const std::vector<DataType>& getFreq(int docId) const
    {
        assert(docId < sizeD && docId >= 0);
        return dataFreq[docId];
    }

    const std::vector<int>& getTermId(int docId) const
    {
        assert(docId < sizeD && docId >= 0);
        return dataTermId[docId];
    }

    int getD() const {
        return sizeD;
    }

    int getW() const {
        return sizeW;
    }

    ~DocWordMatrix() {
        delete [] dataFreq;
        delete [] dataTermId;
    }

    typedef std::auto_ptr<DocWordMatrix> Ptr;
};



#endif // __DOC_WORD_MATRIX
