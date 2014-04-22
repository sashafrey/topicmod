OUT_DIR = ../bin/

all: helpers.cc srcmain.cc
	g++ -g -std=c++11 -o $(OUT_DIR)cpp_client helpers.cc srcmain.cc -I. -I../ -L../bin -lartm -lrpcz -lprotobuf -lglog

clean:
	rm $(OUT_DIR)cpp_client

kos:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$$(pwd)/$(OUT_DIR); \
	$(OUT_DIR)cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

nips:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$$(pwd)/$(OUT_DIR); \
	$(OUT_DIR)cpp_client ../../datasets/docword.nips.txt ../../datasets/vocab.nips.txt 16

debug:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$$(pwd)/$(OUT_DIR); \
	gdb --args $(OUT_DIR)cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

valgrind:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$$(pwd)/$(OUT_DIR); \
	valgrind $(OUT_DIR)cpp_client ../../datasets/docword.kos.txt ../../datasets/vocab.kos.txt 16

