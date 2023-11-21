cmake_minimum_required(VERSION 3.15)
project(MPP)
set(CMAKE_CXX_STANDARD 23)

option(BUILD_TESTS "Build tests" OFF)


if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
else ()
  add_subdirectory(src)
endif()
