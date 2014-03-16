all: helpers.cc srcmain.cc
	g++ -g -std=c++11 -o cpp_client helpers.cc srcmain.cc -I. -I../ -L../../libs -lartm -lrpcz -lprotobuf

clean:
	rm cpp_client

kos:
	./cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

nips:
	./cpp_client ../../datasets/docword.nips.txt ../../datasets/vocab.nips.txt 16

debug:
	gdb --args ./cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

valgrind:
	valgrind ./cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

