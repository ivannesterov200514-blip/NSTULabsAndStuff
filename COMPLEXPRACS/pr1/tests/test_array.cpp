#include <gtest/gtest.h>
#include <stdexcept>
#include "dinarray.h"

TEST(ArrayTest, EmptyArray) {
    Arr arr;
    EXPECT_EQ(arr.arr_getsz(), 0);
}

TEST(ArrayTest, InsertTail) {
    Arr arr;
    Task task{10};
    arr.arr_insert_tail(&task);

    EXPECT_EQ(arr.arr_search(0)->id, 10);
    EXPECT_EQ(arr.arr_getsz(), 1);
}

TEST(ArrayTest, ArrayResize) {
    Arr arr(2);
    Task task1 {10};
    Task task2 {20};
    Task task3 {30};
    arr.arr_insert_tail(&task1);
    arr.arr_insert_tail(&task2);
    arr.arr_insert_tail(&task3);

    EXPECT_EQ(arr.arr_getcap(), 4);
    EXPECT_EQ(arr.arr_search(0)->id, 10);
    EXPECT_EQ(arr.arr_search(1)->id, 20);
    EXPECT_EQ(arr.arr_search(2)->id, 30);
    EXPECT_EQ(arr.arr_getsz(), 3);
}

TEST(ArrayTest, DeleteElement) {
    Arr arr;
    Task task1{10};
    Task task2{20};
    Task task3{30};

    arr.arr_insert_tail(&task1);
    arr.arr_insert_tail(&task2);
    arr.arr_insert_tail(&task3);

    arr.arr_delete_idx(1);

    EXPECT_EQ(arr.arr_getsz(), 2);
    EXPECT_EQ(arr.arr_search(0)->id, 10);
    EXPECT_EQ(arr.arr_search(1)->id, 30);   
}

TEST(ArrayTest, ChangeIDX) {
    Arr arr;
    Task task1{10};
    Task task2{20};

    arr.arr_insert_tail(&task1);
    arr.arr_change_idx(&task2, 0);

    EXPECT_EQ(arr.arr_search(0)->id, 20);
    EXPECT_EQ(arr.arr_getsz(), 1);
}

TEST(ArrayTest, InvalidSearch) {
    Arr arr;

    EXPECT_THROW(arr.arr_search(0), std::out_of_range);
}

TEST(ArrayTest, InvalidDelete) {
    Arr arr;
    
    EXPECT_THROW(arr.arr_delete_idx(0), std::out_of_range);
}

TEST(ArrayTest, NullPointer) {
    Arr arr;
    
    EXPECT_THROW(arr.arr_insert_tail(nullptr), std::invalid_argument);
}

TEST(ArrayTest, InvalidResize) {
    Arr arr;
    Task task1{10};
    Task task2{20};
    Task task3{30};

    arr.arr_insert_tail(&task1);
    arr.arr_insert_tail(&task2);
    arr.arr_insert_tail(&task3);

    EXPECT_THROW(arr.arr_resize(2), std::invalid_argument);
    EXPECT_EQ(arr.arr_getsz(), 3);
    EXPECT_EQ(arr.arr_getcap(), 4);
    EXPECT_EQ(arr.arr_search(0)->id, 10);
    EXPECT_EQ(arr.arr_search(1)->id, 20);
    EXPECT_EQ(arr.arr_search(2)->id, 30);
}

TEST(ArrayTest, NegativeIndex) {
    Arr arr;

    EXPECT_THROW(arr.arr_search(-1), std::out_of_range);
}

TEST(ArrayTest, InvalidChange) {
    Arr arr;
    Task task1{10};
    Task task2{20};

    arr.arr_insert_tail(&task1);

    EXPECT_THROW(arr.arr_change_idx(&task2, 5), std::out_of_range);
}

TEST(ArrayTest, NullChange) {
    Arr arr;
    Task task{10};
    arr.arr_insert_tail(&task);

    EXPECT_THROW(arr.arr_change_idx(nullptr, 0), std::invalid_argument);
}

TEST(ArrayTest, InvalidResizeZero) {
    Arr arr;

    EXPECT_THROW(arr.arr_resize(0), std::invalid_argument);
}