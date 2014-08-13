project(node_controller)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../)
aux_source_directory(. SRC_LIST)

add_executable(node_controller ${SRC_LIST})
target_link_libraries(node_controller artm)
