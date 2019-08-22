cmake_minimum_required(VERSION 3.14)
project(spectral C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(inc)
include_directories(/usr/local/include)

add_library(spectral SHARED src/spectral.c inc/spectral.h)
add_executable(example1 aux/example1.c inc/spectral.h)

# add_link_options()
target_link_libraries(example1 -L/usr/local/lib -lsndfile)
# target_link_options(example1 -m64)
set_target_properties(example1 PROPERTIES COMPILE_FLAGS -m64 LINK_FLAGS -m64 )
target_link_libraries(example1 -L/usr/local/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic)
target_link_libraries(example1 Threads::Threads)

