cmake_minimum_required(VERSION 3.12)
project(swap_cuda)
if(CMAKE_BUILD_TYPE=DEBUG)
    set( CMAKE_CXX_FLAGS "-Wall -O3 " )
endif()

add_subdirectory(src)