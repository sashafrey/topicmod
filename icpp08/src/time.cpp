#include "time.h"

#ifndef _GET_TIME_H
#define _GET_TIME_H

#ifndef NULL
#define NULL 0
#endif

#ifndef WIN32

#include <sys/time.h>

double get_time(){
        timeval tim;
        gettimeofday(&tim,NULL);
        return tim.tv_sec+(tim.tv_usec/1000000.0);
}

#else

double get_time(){
	return 0;
}

#endif

#endif
