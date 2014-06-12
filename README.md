topicmod
========
CMake branch

To build project files:
  1. You should have boost:thread, boost:system, boost:filesystem,
    zeromq, glog, protobuf been installed
  2. Library rpcz should be built from 3rdparty folder and moved
    to ./libs folder

TODO:
  * Add CMake files for tests.
  * Add CMake files for python client
  * Add CMake files to build rpcz and move to ./libs
  * Add CMake files to build protobuf files
  * May be create config file (options to build shared or statis and etc.)
  * Add instructions how to use CMake files
