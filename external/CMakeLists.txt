if(${CMAKE_VERSION} VERSION_LESS 3.14)
  macro(FetchContent_MakeAvailable)
    foreach(contentName IN ITEMS ${ARGV})
        string(TOLOWER ${contentName} contentNameLower)
        FetchContent_GetProperties(${contentName})
        if(NOT ${contentNameLower}_POPULATED)
        FetchContent_Populate(${contentName})
        # Only try to call add_subdirectory() if the populated content
        # can be treated that way. Protecting the call with the check
        # allows this function to be used for projects that just want
        # to ensure the content exists, such as to provide content at
        # a known location.
        if(EXISTS ${${contentNameLower}_SOURCE_DIR}/CMakeLists.txt)
            add_subdirectory(${${contentNameLower}_SOURCE_DIR}
                            ${${contentNameLower}_BINARY_DIR})
        endif()
        endif()
    endforeach()
  endmacro()
endif()

include(FetchContent)

FetchContent_Declare(benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.5.0
)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
)

FetchContent_MakeAvailable(googletest benchmark)