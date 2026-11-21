cmake_minimum_required(VERSION 3.16)

project(TaskManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(
    task_structures
    src/dinarray.cpp
    src/hashtable.cpp
)

target_include_directories(task_structures PUBLIC src)

add_executable(
    test_array
    tests/test_array.cpp
)

target_link_libraries(
    test_array
    PRIVATE
    task_structures
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_array)
