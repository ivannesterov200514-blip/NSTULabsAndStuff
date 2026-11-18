cmake_minimum_required(VERSION 3.16)

project(TaskManager LANGUAGES CSS)

set(cmake_cxx_standart 17)
set (cmake_cxx_standart_required ON)

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
    src/array.cpp
    src/hash.cpp
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