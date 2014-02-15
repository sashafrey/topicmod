all:
	cd src/artm; make all; cd ../..
	cd src/cpp_client; make all; cd ../..

lib:
	cd src/artm; make lib; make all; cd ../..

clean: 
	cd src/artm; make clean; cd ../..
	cd src/cpp_client; make clean; cd ../..

test:
	cd src/artm; make test; cd ../..

proto:
	cd src/artm; make proto; cd ../..

cpp_client:
	cd src/cpp_client; make; cd ../..

cpp_client_test:
	cd src/cpp_client; make test; cd ../..
