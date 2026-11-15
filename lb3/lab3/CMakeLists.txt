cmake_minimum_required(VERSION 3.14)
project(lab3_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# cmake -DENABLE_COVERAGE=ON .. включает измерение покрытия (gcov/lcov)
option(ENABLE_COVERAGE "Build with coverage instrumentation" OFF)

# Все структуры данных (без main.cpp) собираются в одну библиотеку,
# с которой линкуются тесты.
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

enable_testing()

# ---------- Google Test: ulist, dlist ----------
find_package(GTest REQUIRED)
add_executable(test_gtest
    tests/test_ulist.cpp
    tests/test_dlist.cpp
)
target_include_directories(test_gtest PRIVATE tests)
target_link_libraries(test_gtest PRIVATE structs GTest::gtest_main)
add_test(NAME gtest COMMAND test_gtest)

# ---------- Boost.Test: avltree, hashtable ----------
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

# ---------- Catch2 v3: array, stack, queue ----------
find_package(Catch2 3 REQUIRED)
add_executable(test_catch2
    tests/test_array.cpp
    tests/test_stack.cpp
    tests/test_queue.cpp
)
target_include_directories(test_catch2 PRIVATE tests)
target_link_libraries(test_catch2 PRIVATE structs Catch2::Catch2WithMain)
add_test(NAME catch2 COMMAND test_catch2)
