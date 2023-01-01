cmake_minimum_required(VERSION 2.8.13)
project(PlateSolver)
find_package(OpenCV)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} /usr/local/include/opencv4/)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -O3")

file(GLOB SOURCES "src/*.cxx")

add_library(PlateSolver ${SOURCES})

add_executable(tests       "utils/tests.cxx")
add_executable(plate_solve "utils/plate_solve.cxx")
add_dependencies(tests PlateSolver)
add_dependencies(plate_solve PlateSolver)
target_link_libraries(tests PlateSolver ${OpenCV_LIBS})
target_link_libraries(plate_solve PlateSolver ${OpenCV_LIBS})
