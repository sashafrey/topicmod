project(cpp_client)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
aux_source_directory(. SRC_LIST)

add_executable(cpp_client ${SRC_LIST})
target_link_libraries(cpp_client artm)
