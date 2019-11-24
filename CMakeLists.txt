cmake_minimum_required(VERSION 3.11)
project(huffman)

add_subdirectory(${PROJECT_SOURCE_DIR}/external)

find_package(Threads)

add_executable(huffmanbench ${PROJECT_SOURCE_DIR}/test/benchmark.cpp)
target_include_directories(huffmanbench PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(huffmanbench PRIVATE cxx_std_17)
target_link_libraries(huffmanbench benchmark_main Threads::Threads)