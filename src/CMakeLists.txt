project(artm)

# set compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -fPIC -std=c++11")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)

# find Boost
find_package(Boost COMPONENTS thread system filesystem REQUIRED)
if(NOT Boost_FOUND)
  message(SEND_ERROR "Failed to find boost::thread, boost::system or boost::filesystem.")
  return()
else()
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# find Protobuf
find_package(ProtobufPlugin REQUIRED)
if(NOT PROTOBUF_FOUND)
  message(SEND_ERROR "Failed to find protobuf.")
  return()
else()
  include_directories(${PROTOBUF_INCLUDE_DIRS})
endif()

# find Glog
find_package(Glog REQUIRED)
if(NOT GLOG_FOUND)
  message(SEND_ERROR "Failed to find glog.")
  return()
else()
  include_directories(${GLOG_INCLUDE_DIRS})
endif()

# find ZEROMQ
find_package(ZeroMQ REQUIRED)
if(NOT ZeroMQ_FOUND)
  message(SEND_ERROR "Failed to find zeromq.")
  return()
else()
  include_directories(${ZEROMQ_INCLUDE_DIRS})
endif()

# can it be avoided?
include_directories(${3RD_PARTY_DIR}/rpcz/include)
include_directories(${CMAKE_BINARY_DIR}/3rdparty/rpcz/src)

add_subdirectory(artm)
add_subdirectory(artm_tests)
add_subdirectory(cpp_client)
