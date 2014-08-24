project(artm_tests)

include_directories(${3RD_PARTY_DIR}/gtest)
include_directories(${3RD_PARTY_DIR}/gtest/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
aux_source_directory(. SRC_LIST)
set(SRC_LIST ${SRC_LIST} ${3RD_PARTY_DIR}/gtest/src/gtest_main.cc
                         ${3RD_PARTY_DIR}/gtest/src/gtest-all.cc
                         ${CMAKE_SOURCE_DIR}/src/artm/c_interface.cc
                         ${CMAKE_SOURCE_DIR}/src/artm/cpp_interface.cc
                         ${CMAKE_SOURCE_DIR}/src/artm/messages.pb.cc)

add_executable(artm_tests ${SRC_LIST})
target_link_libraries(artm_tests artm-static
                                 rpcz_static
                                 ${Boost_FILESYSTEM_LIBRARIES}
                                 ${Boost_SYSTEM_LIBRARIES}
                                 ${Boost_IOSTREAMS_LIBRARIES}
                                 ${PROTOBUF_LIBRARIES}
                                 ${GLOG_LIBRARIES}
                                 ${ZeroMQ_LIBRARIES})
