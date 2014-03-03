CC=/opt/intel/bin/icpc
FLAGS= -openmp -mkl -I/usr/include/x86_64-linux-gnu/c++/4.8/
objs=PLSACluster.o PCluster.o iPLSA.o NMF.o Schedule.o CBScheduler.o AlignAlloc.o time.o

CC_FLAGS=${CC} ${FLAGS}

PLSACluster : $(objs) Makefile
	$(CC_FLAGS) $(objs) -o PLSACluster
PLSACluster.o : PLSACluster.cpp PCluster.hpp
	$(CC_FLAGS) -c PLSACluster.cpp
PCluster.o : PCluster.cpp iPLSA.hpp NMF.hpp
	$(CC_FLAGS) -c PCluster.cpp
iPLSA.o : iPLSA.cpp 
	$(CC_FLAGS) -c iPLSA.cpp
NMF.o : NMF.cpp NMF.hpp
	$(CC_FLAGS) -c NMF.cpp
Schedule.o : Schedule.cpp Schedule.hpp
	$(CC_FLAGS) -c Schedule.cpp
CBScheduler.o : CBScheduler.cpp CBScheduler.hpp
	$(CC_FLAGS) -c CBScheduler.cpp
AlignAlloc.o : AlignAlloc.cpp AlignAlloc.h
	$(CC_FLAGS) -c AlignAlloc.cpp
time.o : time.cpp time.h
	$(CC_FLAGS) -c time.cpp

clean:
	-rm $(objs) PLSACluster
