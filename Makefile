all: Helpers.cpp PlsaBatchEm.cpp srcMain.cpp
	g++ -g -std=c++11 -o topicmd helpers.cpp srcmain.cpp -I. -lprotobuf -ltopicmd

lib:
	cd src/topicmd; make; cd ../..

clean: 
	rm topicmd

test:
	./test/topicmd datasets/docword.kos.txt datasets/vocab.kos.txt 16

proto:
	cd src/topicmd; make proto; cd ../..

.PHONY: all clean test
