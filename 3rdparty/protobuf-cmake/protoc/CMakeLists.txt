project( protoc )

include( ../protobuf-common.cmake )

set(SOURCES
  ${PROTOBUF_ROOT}/src/google/protobuf/compiler/main.cc
)

add_definitions(
  -D_CONSOLE
)

add_executable(protoc ${SOURCES})
target_link_libraries( protoc libprotoc libprotobuf )
if( UNIX )
  target_link_libraries( protoc -lpthread )
endif()

if (UNIX) 
  set(PROTOBUF_PROTOC_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/protoc CACHE INTERNAL "protoc build")
else()
  set(PROTOBUF_PROTOC_EXECUTABLE ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/protoc.exe CACHE INTERNAL "protoc build")
endif()
  

install(TARGETS protoc
    RUNTIME DESTINATION ${BIN_DIR}
    )
