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

add_executable(merge_index_files "utils/merge_index_files.cxx")
add_dependencies(merge_index_files PlateSolver)
target_link_libraries(merge_index_files PlateSolver ${OpenCV_LIBS})

add_executable(plate_solve "utils/plate_solve.cxx")
add_dependencies(plate_solve PlateSolver)
target_link_libraries(plate_solve PlateSolver ${OpenCV_LIBS})

add_executable(convert_catalogue_csv_to_bin "utils/convert_catalogue_csv_to_bin.cxx")
add_dependencies(convert_catalogue_csv_to_bin PlateSolver)
target_link_libraries(convert_catalogue_csv_to_bin PlateSolver ${OpenCV_LIBS})

# Build the Python module around the C++ shared library.
add_library( plateSolvingm SHARED utils/python_binding.cxx )
add_dependencies(plateSolvingm PlateSolver)
target_link_libraries( plateSolvingm PRIVATE Python3::Python ${Python3_LIBRARIES}  PlateSolver ${OpenCV_LIBS})
set_target_properties( plateSolvingm PROPERTIES
   PREFIX ""
   OUTPUT_NAME "plateSolving" )

# Build the Python module around the C++ shared library.
add_library( cppLogger SHARED utils/python_binding_logger.cxx )
add_dependencies(cppLogger PlateSolver)
target_link_libraries( cppLogger PRIVATE Python3::Python ${Python3_LIBRARIES}  PlateSolver ${OpenCV_LIBS})
set_target_properties( cppLogger PROPERTIES
   PREFIX ""
   OUTPUT_NAME "cppLogger" )
