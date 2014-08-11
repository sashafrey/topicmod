
# - Try to find Gtest
#
# The following variables are optionally searched for defaults
#  GLOG_ROOT_DIR:            Base directory where all GLOG components are found
#
# The following are set after configuration is done: 
#  GLOG_FOUND
#  GLOG_INCLUDE_DIRS
#  GLOG_LIBRARIES
#  GLOG_LIBRARYRARY_DIRS

include(FindPackageHandleStandardArgs)

set(GTEST_ROOT_DIR ${3RD_PARTY_DIR}/gtest CACHE PATH "Folder contains Google gtest")

find_path(GTEST_INCLUDE_DIR gtest/gtest.h
    PATHS ${GTEST_ROOT_DIR}/include)

set(GTEST_LIBRARY gtest CACHE INTERNAL "gtest library")

find_package_handle_standard_args(GTEST DEFAULT_MSG
    GTEST_INCLUDE_DIR GTEST_LIBRARY)

if(GTEST_FOUND)
    set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
    set(GTEST_LIBRARIES ${GTEST_LIBRARY})
endif()
