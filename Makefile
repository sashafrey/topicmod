lib:
	cd src/topicmd; make lib; make all; cd ../..

clean: 
	cd src/topicmd; make clean; cd ../..
	cd src/cpp_client; make clean; cd ../..

test:
	cd src/topicmd; make test; cd ../..

proto:
	cd src/topicmd; make proto; cd ../..

cpp_client:
	cd src/cpp_client; make; cd ../..

cpp_client_test:
	cd src/cpp_client; make test; cd ../..
