cmake_minimum_required(VERSION 2.8)

project(BigARTM)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)
set(BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBS_DIR})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)

set(3RD_PARTY_DIR ${CMAKE_SOURCE_DIR}/3rdparty)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
message("-- Build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(3rdparty)
add_subdirectory(src)
