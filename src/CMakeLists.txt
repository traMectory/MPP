cmake_minimum_required(VERSION 3.27)
set (CMAKE_CXX_STANDARD 23)
project(MPP)
file (GLOB_RECURSE sources *.h *.cpp *.hpp *.tpp)

find_package(CGAL REQUIRED)
include_directories(include tools)

add_executable(mpp ${sources})
target_link_libraries(mpp CGAL::CGAL)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Clipper2 REQUIRED IMPORTED_TARGET Clipper2)
target_link_libraries(mpp PkgConfig::Clipper2)