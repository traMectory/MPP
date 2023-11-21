file (GLOB_RECURSE sources *.h *.cpp *.hpp *.tpp)

option(CXX "enable C++ compilation" ON)

if(CXX)
  enable_language(CXX)
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(CGAL)

include_directories(include)
include_directories(include/solvers)
include_directories(tools)
add_executable(mpp ${sources})

if(CXX)
  set(CMAKE_CXX_STANDARD 23)
endif()


target_link_libraries(mpp CGAL::CGAL)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  include(FeatureSummary)
  feature_summary(WHAT ALL)
endif()