cmake_minimum_required(VERSION 3.27)
set (CMAKE_CXX_STANDARD 23)
project(MPP)
file (GLOB_RECURSE sources *.h *.cpp *.hpp *.tpp)

find_package(CGAL REQUIRED)
find_package(CGAL COMPONENTS Qt5)
add_definitions(-DCGAL_USE_BASIC_VIEWER)
include_directories(include tools)

add_executable(mpp ${sources})
target_link_libraries(mpp CGAL::CGAL_Qt5)