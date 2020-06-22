
add_executable(cuda-version main.cpp cuda_finder.cpp)
find_package(Threads REQUIRED)

target_link_libraries(cuda-version Threads::Threads -lstdc++fs)
target_include_directories(cuda-version PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET cuda-version PROPERTY CXX_STANDARD 17)

if(${CMAKE_CXX_COMPILER_VERSION} LESS 8)
    add_compile_definitions(GCC7)
endif(${CMAKE_CXX_COMPILER_VERSION} LESS 8)


install(TARGETS cuda-version DESTINATION "bin")