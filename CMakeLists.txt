cmake_minimum_required(VERSION 3.11)
project(huffman)

add_subdirectory(${PROJECT_SOURCE_DIR}/external)

find_package(Threads)

add_library(mementar STATIC ${PROJECT_SOURCE_DIR}/src/mementar/mementar.cpp)
target_include_directories(mementar PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(jcbasic STATIC ${PROJECT_SOURCE_DIR}/src/jcbasic/jcbasic.cpp)
target_include_directories(jcbasic PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jcbasic PUBLIC cxx_std_17)

add_library(jcindex INTERFACE)
target_include_directories(jcbasic PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jcbasic PUBLIC cxx_std_17)

add_executable(huffmanbench ${PROJECT_SOURCE_DIR}/test/benchmark.cpp)
target_link_libraries(huffmanbench 
    mementar jcbasic jcindex
    benchmark_main 
    Threads::Threads
)