all: helpers.cc srcmain.cc
	g++ -g -std=c++11 -o cpp_client helpers.cc srcmain.cc -I. -I../ -lprotobuf -L../../libs -lartm

clean:
	rm cpp_client

kos:
	./cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

nips:
	./cpp_client ../../datasets/docword.nips.txt ../../datasets/vocab.nips.txt 16

debug:
	gdb --args ./cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

