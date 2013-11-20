all: Helpers.cpp PlsaBatchEm.cpp srcMain.cpp
	g++ -g -o topicmd Helpers.cpp PlsaBatchEm.cpp srcMain.cpp api/api.pb.cc -I. -lprotobuf 

clean: 
	rm topicmd

tests:
	./topicmd ../datasets/docword.kos.txt ../datasets/vocab.kos.txt 16
