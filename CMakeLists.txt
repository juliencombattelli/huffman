cmake_minimum_required(VERSION 3.11)
project(huffman)

add_subdirectory(${PROJECT_SOURCE_DIR}/external)

find_package(Threads)
find_package(Boost COMPONENTS program_options REQUIRED)

add_library(mementar STATIC ${PROJECT_SOURCE_DIR}/src/mementar/mementar.cpp)
target_include_directories(mementar PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(jcbasic_fastercount STATIC ${PROJECT_SOURCE_DIR}/src/jcbasic_fastercount/jcbasic_fastercount.cpp)
target_include_directories(jcbasic_fastercount PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jcbasic_fastercount PUBLIC cxx_std_17)

add_library(jcbasic STATIC ${PROJECT_SOURCE_DIR}/src/jcbasic/jcbasic.cpp)
target_include_directories(jcbasic PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jcbasic PUBLIC cxx_std_17)

add_library(jcindex INTERFACE)
target_include_directories(jcindex INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jcindex INTERFACE cxx_std_17)

add_library(jcindex_fastercount INTERFACE)
target_include_directories(jcindex_fastercount INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jcindex_fastercount INTERFACE cxx_std_17)

add_library(jcdod STATIC ${PROJECT_SOURCE_DIR}/src/jcdod/jcdod.cpp)
target_include_directories(jcdod PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(jcdod PUBLIC cxx_std_17)

add_executable(jcdodtest ${PROJECT_SOURCE_DIR}/test/test_jcdod.cpp)
target_link_libraries(jcdodtest 
    jcdod
    Threads::Threads
)

add_executable(huffmanbench ${PROJECT_SOURCE_DIR}/test/benchmark.cpp)
target_link_libraries(huffmanbench 
    mementar 
    jcbasic jcbasic_fastercount 
    jcindex jcindex_fastercount
    jcdod
    benchmark_main 
    Threads::Threads
    Boost::program_options
)