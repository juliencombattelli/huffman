cmake_minimum_required(VERSION 3.11)
project(huffman)

add_subdirectory(${PROJECT_SOURCE_DIR}/external)

find_package(Threads)
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(mementar STATIC ${PROJECT_SOURCE_DIR}/src/mementar/mementar.cpp)
target_include_directories(mementar PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(jc STATIC ${PROJECT_SOURCE_DIR}/src/jc/jc.cpp)
target_include_directories(jc PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jc PUBLIC cxx_std_17)

add_executable(huffmanbench ${PROJECT_SOURCE_DIR}/test/benchmark.cpp)
target_link_libraries(huffmanbench 
    mementar 
    jc
    benchmark_main 
    Threads::Threads
    Boost::program_options
)