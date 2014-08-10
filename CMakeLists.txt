cmake_minimum_required(VERSION 2.8)

project(BigARTM)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

if (WIN32)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

if (WIN32)
  set(BOOST_INCLUDEDIR "C:/local/boost_1_55_0")
  set(BOOST_LIBRARYDIR "C:/local/boost_1_55_0/lib32-msvc-11.0")
  add_definitions("-D_WIN32")
  add_definitions("-DWIN32")
endif()

if (MSVC)
  add_definitions("-D_VARIADIC_MAX=10")
  add_definitions("-DBOOST_ALL_DYN_LINK")
  add_definitions("-DBOOST_LIB_DIAGNOSTIC")
  add_definitions("/wd4251")
endif()

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS OFF)

set(3RD_PARTY_DIR ${CMAKE_SOURCE_DIR}/3rdparty)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
message("-- Build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(3rdparty)
add_subdirectory(src)
