cmake_minimum_required(VERSION 3.11)
project(huffman)

add_subdirectory(${PROJECT_SOURCE_DIR}/external)

find_package(Threads)
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(mementar STATIC ${PROJECT_SOURCE_DIR}/src/mementar/mementar.cpp)
target_include_directories(mementar PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_library(jc_vec STATIC ${PROJECT_SOURCE_DIR}/src/jc_vec/jc_vec.cpp)
target_include_directories(jc_vec PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_features(jc_vec PUBLIC cxx_std_17)

add_library(jc_arr STATIC ${PROJECT_SOURCE_DIR}/src/jc_arr/jc_arr.cpp)
target_include_directories(jc_arr PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_features(jc_arr PUBLIC cxx_std_17)

add_executable(huffmanbench ${PROJECT_SOURCE_DIR}/test/benchmark.cpp)
target_link_libraries(huffmanbench 
    mementar 
    jc_vec jc_arr
    benchmark_main 
    Threads::Threads
    Boost::program_options
)