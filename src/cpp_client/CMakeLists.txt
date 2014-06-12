cmake_minimum_required(VERSION 2.8)

set(EXEC_NAME cpp_client)
project(${EXEC_NAME})

MESSAGE(STATUS "cpp_client")

# define output directory
if(BIN_DIR)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
  MESSAGE(STATUS "GOOD")
else()
  MESSAGE(STATUS "BAD")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

# compiler oprions
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

# find Boost
find_package(Boost COMPONENTS filesystem system REQUIRED)
if(NOT Boost_FOUND)
  message(SEND_ERROR "Failed to find boost::filesystem or boost::system.")
  return()
else()
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# find Protobuf
find_package(Protobuf REQUIRED)
if(NOT PROTOBUF_FOUND)
  message(SEND_ERROR "Failed to find protobuf")
  return()
else()
  include_directories(${PROTOBUF_INCLUDE_DIRS})
endif()

# find Glog
find_package(Glog REQUIRED)
if(NOT GLOG_FOUND)
  message(SEND_ERROR "Failed to find glog")
  return()
else()
  include_directories(${GLOG_INCLUDE_DIRS})
endif()

# includes
include_directories(${PROJECT_SOURCE_DIR}/../)
include_directories(${3RD_PARTY_DIR}/rpcz/include)
include_directories(${3RD_PARTY_DIR}/rpcz/src)

# make source list
include_directories(${CMAKE_CURRENT_LIST_DIR})
aux_source_directory(. SRC_LIST)

# library deps
set(CPP_CLIENT_LIB_DEPS  ${LIBS_DIR}/libARTM.so
                         ${LIBS_DIR}/librpcz.so
                         ${Boost_FILESYSTEM_LIBRARIES}
                         ${Boost_SYSTEM_LIBRARIES}
                         ${PROTOBUF_LIBRARIES}
                         ${GLOG_LIBRARIES}
                         ${ZeroMQ_LIBRARIES})

add_executable(${EXEC_NAME} ${SRC_LIST})
target_link_libraries(${EXEC_NAME} ${CPP_CLIENT_LIB_DEPS})
