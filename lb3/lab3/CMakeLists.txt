cmake_minimum_required(VERSION 3.14)
project(lab3_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_COVERAGE "Build with coverage instrumentation" OFF)
option(BUILD_UNIT_TESTS "Build GTest, Boost.Test and Catch2 tests" ON)

add_library(structs STATIC
    array.cpp
    avltree.cpp
    dlist.cpp
    hashtable.cpp
    queue.cpp
    stack.cpp
    ulist.cpp
)
target_include_directories(structs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(structs PRIVATE -Wall -Wextra)

if(ENABLE_COVERAGE)
    target_compile_options(structs PUBLIC --coverage -O0 -g)
    target_link_options(structs PUBLIC --coverage)
endif()

add_executable(dbms main.cpp binary_storage.cpp)
target_link_libraries(dbms PRIVATE structs)

add_executable(benchmark_cpp bench/benchmark.cpp)
target_link_libraries(benchmark_cpp PRIVATE structs)
target_compile_options(benchmark_cpp PRIVATE -O2)

add_executable(test_safety tests/test_safety.cpp)
target_link_libraries(test_safety PRIVATE structs)

enable_testing()
add_test(NAME safety COMMAND test_safety)
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    add_test(NAME cli_roundtrip COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.py $<TARGET_FILE:dbms>)
endif()

if(BUILD_UNIT_TESTS)

find_package(GTest REQUIRED)
add_executable(test_gtest
    tests/test_ulist.cpp
    tests/test_dlist.cpp
)
target_include_directories(test_gtest PRIVATE tests)
target_link_libraries(test_gtest PRIVATE structs GTest::gtest_main)
add_test(NAME gtest COMMAND test_gtest)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
add_executable(test_boost
    tests/boost_main.cpp
    tests/test_avltree.cpp
    tests/test_hashtable.cpp
)
target_include_directories(test_boost PRIVATE tests)
target_compile_definitions(test_boost PRIVATE BOOST_TEST_DYN_LINK)
target_link_libraries(test_boost PRIVATE structs Boost::unit_test_framework)
add_test(NAME boost COMMAND test_boost)

find_package(Catch2 3 REQUIRED)
add_executable(test_catch2
    tests/test_array.cpp
    tests/test_stack.cpp
    tests/test_queue.cpp
)
target_include_directories(test_catch2 PRIVATE tests)
target_link_libraries(test_catch2 PRIVATE structs Catch2::Catch2WithMain)
add_test(NAME catch2 COMMAND test_catch2)

endif() 
