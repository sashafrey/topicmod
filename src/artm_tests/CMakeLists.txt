project(artm_tests)

include_directories(${3RD_PARTY_DIR}/gtest)
include_directories(${3RD_PARTY_DIR}/gtest/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
aux_source_directory(. SRC_LIST)
set(SRC_LIST ${SRC_LIST} ${3RD_PARTY_DIR}/gtest/src/gtest_main.cc ${3RD_PARTY_DIR}/gtest/src/gtest-all.cc)

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

add_executable(artm_tests ${SRC_LIST})
target_link_libraries(artm_tests ${CPP_CLIENT_LIB_DEPS})
