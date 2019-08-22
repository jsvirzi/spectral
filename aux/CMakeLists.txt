cmake_minimum_required(VERSION 3.14)
project(aux)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

include_directories(../inc)
include_directories(/usr/local/include)

add_executable(example1 example1.cpp)
# add_link_options(-L../lib -lspectral)
target_link_libraries(example1 -L/home/jsvirzi/projects/spectral/lib -lspectral)
# target_link_libraries(example1 ../lib/libspectral.so)
target_link_libraries(example1 -L/usr/local/lib -lsndfile)
target_link_libraries(example1 -L/usr/local/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic)

