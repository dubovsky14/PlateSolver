cmake_minimum_required(VERSION 2.8.13)
project(PlateSolver)
find_package(OpenCV)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} /usr/local/include/opencv4/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -O3")

file(GLOB SOURCES "src/*.cxx")

add_library(PlateSolver ${SOURCES})

add_executable(tests ${CI_TESTS_SOURCES} "utils/tests.cxx")
add_dependencies(tests PlateSolver)
target_link_libraries(tests PlateSolver ${OpenCV_LIBS})
