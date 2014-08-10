project(cpp_client)

# can it be avoided?
#include_directories(${3RD_PARTY_DIR}/rpcz/include)
#include_directories(${3RD_PARTY_DIR}/rpcz/src)
#include_directories(${CMAKE_BINARY_DIR}/3rdparty/gflags/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)

# make source list
include_directories(${CMAKE_CURRENT_LIST_DIR})
#include_directories(${CMAKE_BINARY_DIR}/src)
aux_source_directory(. SRC_LIST)

if(WIN32)
  set(RPCZ_LIBS rpcz_static)
else()
  set(RPCZ_LIBS rpcz)
endif()

# library deps
set(CPP_CLIENT_LIB_DEPS  artm_shared
                         ${RPCZ_LIBS}
                         ${Boost_FILESYSTEM_LIBRARIES}
                         ${Boost_SYSTEM_LIBRARIES}
                         ${PROTOBUF_LIBRARIES}
                         ${GLOG_LIBRARIES}
                         ${ZeroMQ_LIBRARIES})

add_executable(cpp_client ${SRC_LIST})
target_link_libraries(cpp_client ${CPP_CLIENT_LIB_DEPS})
