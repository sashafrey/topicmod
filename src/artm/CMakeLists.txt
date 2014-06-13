cmake_minimum_required(VERSION 2.8)

set(LIBRARY_NAME ARTM)
project(${LIBRARY_NAME})

# set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -fPIC -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Werror")

# find Boost
find_package(Boost COMPONENTS thread system REQUIRED)
if(NOT Boost_FOUND)
  message(SEND_ERROR "Failed to find boost::thread or boost::system.")
  return()
else()
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# find Protobuf
find_package(Protobuf REQUIRED)
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

# find ZeroMQ
find_package(ZeroMQ REQUIRED)
if(NOT ZeroMQ_FOUND)
  message(SEND_ERROR "Failed to find zeromq.")
  return()
else()
  include_directories(${ZeroMQ_INCLUDE_DIRS})
endif()

include_directories(${3RD_PARTY_DIR}/rpcz/include)
include_directories(${3RD_PARTY_DIR}/rpcz/src)

# make source list
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
include_directories(${CMAKE_CURRENT_LIST_DIR}/core)
include_directories(${CMAKE_CURRENT_LIST_DIR}/regularizer) # which one?
include_directories(${CMAKE_CURRENT_LIST_DIR}/regularizer_sandbox) # ??
aux_source_directory(. CUR_LIST)
aux_source_directory(./core CORE_SRC_LIST)
aux_source_directory(./regularizer REG_SRC_LIST)
aux_source_directory(./regularizer_sandbox REG2_SRC_LIST)
set(SRC_LIST ${CUR_LIST} 
             ${CORE_SRC_LIST} 
             ${REG_SRC_LIST} 
             ${REG2_SRC_LIST})

# library deps
set(ARTM_LIB_DEPS  rpcz
                   ${Boost_THREAD_LIBRARIES}
                   ${Boost_SYSTEM_LIBRARIES}
                   ${PROTOBUF_LIBRARIES}
                   ${GLOG_LIBRARIES}
                   ${ZeroMQ_LIBRARIES})

add_library(${LIBRARY_NAME} SHARED ${SRC_LIST})
target_link_libraries(${LIBRARY_NAME} ${ARTM_LIB_DEPS})
