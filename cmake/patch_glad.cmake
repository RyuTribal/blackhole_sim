if(NOT DEFINED GLAD_SOURCE)
  message(FATAL_ERROR "GLAD_SOURCE not provided to patch_glad.cmake")
endif()

set(_glad_cmake "${GLAD_SOURCE}/CMakeLists.txt")
if(NOT EXISTS "${_glad_cmake}")
  message(FATAL_ERROR "Cannot find glad CMakeLists.txt at ${_glad_cmake}")
endif()

file(READ "${_glad_cmake}" _glad_contents)
string(REPLACE "cmake_minimum_required(VERSION 3.0)" "cmake_minimum_required(VERSION 3.21)" _glad_contents "${_glad_contents}")
file(WRITE "${_glad_cmake}" "${_glad_contents}")
