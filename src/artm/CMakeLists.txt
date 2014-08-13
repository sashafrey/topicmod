include_directories(${CMAKE_CURRENT_LIST_DIR}/..)

# Static 'artm' library
FILE(GLOB_RECURSE SRC_LIST *.cc)
list(REMOVE_ITEM SRC_LIST c_interface.cc cpp_interface.cc)
add_library(artm-static STATIC ${SRC_LIST})

# Shared 'artm' library
set(SRC_LIST "c_interface.cc")

set(ARTM_LIB_DEPS  artm-static
		   gflags-static
                   rpcz_static
                   ${Boost_THREAD_LIBRARIES}
                   ${Boost_SYSTEM_LIBRARIES}
                   ${Boost_FILESYSTEM_LIBRARIES}
                   ${PROTOBUF_LIBRARIES}
                   ${GLOG_LIBRARIES}
                   ${ZEROMQ_LIBRARIES})

add_library(artm SHARED ${SRC_LIST})

target_link_libraries(artm ${ARTM_LIB_DEPS})
