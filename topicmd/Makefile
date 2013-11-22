all: Helpers.cpp PlsaBatchEm.cpp srcMain.cpp
	g++ -g -std=c++11 -o topicmd Helpers.cpp PlsaBatchEm.cpp srcMain.cpp api/api.pb.cc topicmd_api.cpp -I. -lprotobuf 

clean: 
	rm topicmd

tests:
	./topicmd ../datasets/docword.kos.txt ../datasets/vocab.kos.txt 16
