cmake_minimum_required(VERSION 3.14)
project(spectral C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

include_directories(inc)

add_library(spectral SHARED src/spectral.c inc/spectral.h)
add_executable(example1 aux/example1.c inc/spectral.h)

# add_link_options()
target_link_libraries(example1 -L/usr/local/lib -lsndfile)
