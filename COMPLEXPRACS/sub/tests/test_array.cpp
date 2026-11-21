#include <gtest/gtest.h>
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
}