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
include(GoogleTest)

add_library(
    task_structures
    src/dinarray.cpp
    src/table.cpp
    src/heap.cpp
    src/taskmanager.cpp
)

target_include_directories(task_structures PUBLIC src)

add_executable(taskctl src/main.cpp)

target_link_libraries(taskctl PRIVATE task_structures)

add_executable(test_array tests/test_array.cpp)

target_link_libraries(
    test_array
    PRIVATE
    task_structures
    GTest::gtest_main
)

add_executable(test_hashtable tests/test_hashtable.cpp)

target_link_libraries(
    test_hashtable
    PRIVATE
    task_structures
    GTest::gtest_main
)

add_executable(test_heap tests/test_heap.cpp)

target_link_libraries(
    test_heap
    PRIVATE
    task_structures
    GTest::gtest_main
)

add_executable(test_taskmanager tests/test_taskmanager.cpp)

target_link_libraries(test_taskmanager
    PRIVATE
    task_structures
    GTest::gtest_main
)

gtest_discover_tests(test_taskmanager)
gtest_discover_tests(test_heap)
gtest_discover_tests(test_hashtable)
gtest_discover_tests(test_array)