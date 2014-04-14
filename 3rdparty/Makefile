proto-install :
	cd protobuf; \
	chmod 700 configure; \
	./configure; \
	make; \
	make check; \
	sudo make install; \
	make clean; \
	export LD_LIBRARY_PATH="/usr/local/lib"; \

zeromq-install :
	mkdir ~/zeromq; \
	cd ~/zeromq; \
	wget http://download.zeromq.org/zeromq-4.0.4.tar.gz; \
	tar -xzf zeromq-4.0.4.tar.gz; \
	cd zeromq-4.0.4/; \
	export ZEROMQ_ROOT=~/zeromq/zeromq-4.0.4/; \
	./autoget.sh; \
	./configure; \
	make; \
	sudo make install; \

rpcz-install :
	cd rpcz; \
	mkdir build; \
	cd build; \
	cmake .. -Drpcz_build_examples=0 -DZeroMQ_INCLUDE_DIR="$ZEROMQ_ROOT/include"; \
	make; \
	sudo make install; \
	make clean; \

glog-install :
	cd glog; \
	chmod 700 configure; \
	./configure; \
	make; \
	sudo make install; \
	make clean; \
