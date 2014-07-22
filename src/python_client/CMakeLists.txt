cmake_minimum_required(VERSION 2.8)

project(python_client)

# set python version
set(PythonInterp_FIND_VERSION 2)

find_package(PythonInterp REQUIRED)
if(NOT PYTHONINTERP_FOUND)
  message(SEND_ERROR "Failed to find python interpreter.")
  return()
endif()

set(PyEXEC ${CMAKE_CURRENT_LIST_DIR}/python_client.py)
set(PySRC ${CMAKE_CURRENT_LIST_DIR}/python_client.py
          ${CMAKE_CURRENT_LIST_DIR}/../python_interface/python_interface.py
          ${CMAKE_CURRENT_LIST_DIR}/../python_interface/messages_pb2.py)

add_custom_target(python_client ${PYTHON_EXECUTABLE} ${PyEXEC} 
                                SOURCES ${PySRC}
                                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
