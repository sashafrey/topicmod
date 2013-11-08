#include <memory>

#include "AlignAlloc.h"

#define ALIGN_BYTES 128

#include <iostream>
using namespace std;

char *AlignAlloc::al_alloc(unsigned long size){
//    char *oriP= (char *)malloc(size + ALIGN_BYTES);
	char * oriP=(char *)new char[size+ALIGN_BYTES];
    char *alignP = oriP + ALIGN_BYTES - ((long)oriP & (ALIGN_BYTES-1));
    ((void **)alignP)[-1] = oriP;
    return alignP;
}

void AlignAlloc::al_free(void *p){;
//    free(((void **)p)[-1]);
	delete[] ((void **)p)[-1];
}
