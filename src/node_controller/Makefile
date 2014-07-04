OUT_DIR = ../bin/

all: node_controller.cc
	g++ -g -std=c++11 -o $(OUT_DIR)node_controller node_controller.cc -I. -I../ -L../bin -lartm -lrpcz -lprotobuf -lglog -lboost_system -lboost_filesystem

clean:
	rm $(OUT_DIR)node_controller

run:
	export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$$(pwd)/$(OUT_DIR); \
	$(OUT_DIR)node_controller tcp://*:5556
