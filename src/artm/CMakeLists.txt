cmake_minimum_required(VERSION 2.8)

set(LIBRARY_NAME ARTM)
project(${LIBRARY_NAME})

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
include_directories(${CMAKE_BINARY_DIR}/3rdparty/gflags/include)

protobuf_generate_cpp(ARTM_PB_SRCS ARTM_PB_HEADERS
    proto/messages.proto
    proto/internals.proto
)
set(ARTM_PROTO_SOURCES ${ARTM_PB_SRCS} ${ARTM_PB_HEADERS})

# make source list
include_directories(${CMAKE_BINARY_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/src/artm)
include_directories(${CMAKE_BINARY_DIR}/src/artm/core)
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
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
             ${REG2_SRC_LIST}
             ${ARTM_PROTO_SOURCES}
)

if(WIN32)
  set(RPCZ_LIBS rpcz_static)
else()
  set(RPCZ_LIBS rpcz)
endif()

# library deps
set(ARTM_LIB_DEPS  ${RPCZ_LIBS}
		   gflags-shared
                   ${Boost_THREAD_LIBRARIES}
                   ${Boost_SYSTEM_LIBRARIES}
                   ${Boost_FILESYSTEM_LIBRARIES}
                   ${PROTOBUF_LIBRARIES}
                   ${GLOG_LIBRARIES}
                   ${ZEROMQ_LIBRARIES})

add_library(${LIBRARY_NAME} SHARED ${SRC_LIST})
target_link_libraries(${LIBRARY_NAME} ${ARTM_LIB_DEPS})

if(MSVC AND NOT Boost_USE_STATIC_LIBS) 
  get_filename_component(BOOST_THREAD_PATH ${Boost_THREAD_LIBRARY_RELEASE} NAME_WE)
  get_filename_component(BOOST_SYSTEM_PATH ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE)
  get_filename_component(BOOST_FILESYSTEM_PATH ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE)
  file(COPY
    ${BOOST_LIBRARYDIR}/${BOOST_THREAD_PATH}.dll
    ${BOOST_LIBRARYDIR}/${BOOST_SYSTEM_PATH}.dll
    ${BOOST_LIBRARYDIR}/${BOOST_FILESYSTEM_PATH}.dll
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
  ) 
endif()
