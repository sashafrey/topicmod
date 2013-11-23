all: Helpers.cpp PlsaBatchEm.cpp srcMain.cpp
	g++ -g -std=c++11 -o topicmd helpers.cpp srcmain.cpp -I. -lprotobuf -ltopicmd

lib:
	cd topicmd; make; cd ..

clean: 
	rm topicmd

test:
	./test/topicmd datasets/docword.kos.txt datasets/vocab.kos.txt 16

proto:
	protoc --cpp_out=. messages.proto
	protoc --python_out=. messages.proto
	protoc --java_out=./.. messages.proto

.PHONY: all clean test
