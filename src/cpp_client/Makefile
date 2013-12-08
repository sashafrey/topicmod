all: helpers.cc srcmain.cc
	g++ -g -std=c++11 -o cpp_client helpers.cc srcmain.cc -I. -I../ -lprotobuf -ltopicmd

clean:
	rm cpp_client

test:
	./cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

debug:
	gdb --args ./cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

