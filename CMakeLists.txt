cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(3RD_PARTY_DIR ${CMAKE_SOURCE_DIR}/3rdparty)

#add_subdirectory(3rdparty)
add_subdirectory(src)
