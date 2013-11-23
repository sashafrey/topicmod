#ifndef __DOC_TOPIC_MATRIX
#define __DOC_TOPIC_MATRIX

#include <assert.h>
#include <stdlib.h>

#include "common.h"

class DocTopicMatrix 
{
private:
    DISALLOW_COPY_AND_ASSIGN(DocTopicMatrix);
    int sizeD;
    int sizeT;
    DataType* data;
public:
    DocTopicMatrix(int _sizeD, int _sizeT) : sizeD(_sizeD), sizeT(_sizeT) 
    {
        assert(_sizeD > 0);
        assert(_sizeT > 0);

        // set up data and testData.
        data = new DataType[ sizeD * sizeT ];
    }

    ~DocTopicMatrix() {
        delete [] data;
    }

    const DataType* get() const {
        return data;
    }

    DataType* get() {
        return data;
    }

    typedef std::auto_ptr<DocTopicMatrix> Ptr;
};

#endif // __DOC_TOPIC_MATRIX
