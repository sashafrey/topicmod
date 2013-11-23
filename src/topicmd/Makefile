lib:
	g++ -Wall -shared -fPIC -o libtopicmd.so messages.pb.cc cpp_interface.cc c_interface.cc 
	sudo mv libtopicmd.so /usr/lib/

proto:
	protoc --cpp_out=. messages.proto
	protoc --python_out=. messages.proto
	protoc --java_out=./.. messages.proto

