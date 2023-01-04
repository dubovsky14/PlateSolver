cmake_minimum_required(VERSION 2.8.13)
project(PlateSolver)
find_package(OpenCV)

# needed for python wrapper
find_package( Python3 COMPONENTS Development REQUIRED )

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-Wall -O3")

file(GLOB SOURCES "src/*.cxx")

add_library(PlateSolver SHARED ${SOURCES})

add_executable(create_hash_file "utils/create_hash_file.cxx")
add_dependencies(create_hash_file PlateSolver)
target_link_libraries(create_hash_file PlateSolver ${OpenCV_LIBS})

add_executable(plate_solve "utils/plate_solve.cxx")
add_dependencies(plate_solve PlateSolver)
target_link_libraries(plate_solve PlateSolver ${OpenCV_LIBS})


add_executable(convert_text_hash_file_to_binary "utils/convert_text_hash_file_to_binary.cxx")
add_dependencies(convert_text_hash_file_to_binary PlateSolver)
target_link_libraries(convert_text_hash_file_to_binary PlateSolver ${OpenCV_LIBS})

# Build the Python module around the C++ shared library.
add_library( plateSolvingm SHARED utils/python_binding.cxx )
add_dependencies(plateSolvingm PlateSolver)
target_link_libraries( plateSolvingm PRIVATE Python3::Python ${Python3_LIBRARIES}  PlateSolver ${OpenCV_LIBS})
set_target_properties( plateSolvingm PROPERTIES
   PREFIX ""
   OUTPUT_NAME "plateSolving" )
