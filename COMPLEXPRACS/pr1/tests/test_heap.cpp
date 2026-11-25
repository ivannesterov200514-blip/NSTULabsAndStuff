#include <gtest/gtest.h>
#include <stdexcept>
#include "heap.h"

TEST(HeapTest, EmptyHeap) {
    Heap heap;
    EXPECT_EQ(heap.heap_getsz(), 0);
}

TEST(HeapTest, HeapPush) {
    Heap heap;
    HeapNode task1{10, 80, 0};
    HeapNode task2{20, 80, 1};
    HeapNode task3{30, 90, 2};
    
    heap.heap_push(&task1);
    heap.heap_push(&task2);
    heap.heap_push(&task3);

    EXPECT_EQ(heap.heap_getsz(), 3);
    EXPECT_EQ(heap.heap_pop().id, 30);
    EXPECT_EQ(heap.heap_pop().id, 10);
    EXPECT_EQ(heap.heap_pop().id, 20);
    EXPECT_EQ(heap.heap_getsz(), 0);
}

TEST(HeapTest, HeapPeek) {
    Heap heap;
    HeapNode task1{10, 50, 0};
    HeapNode task2{20, 90, 1};

    heap.heap_push(&task1);
    heap.heap_push(&task2);

    EXPECT_EQ(heap.heap_peek().id, 20);
    EXPECT_EQ(heap.heap_getsz(), 2);
}

TEST(HeapTest, EmptyPeek) {
    Heap heap;
    EXPECT_THROW(heap.heap_peek(), std::out_of_range);
}

TEST(HeapTest, EmptyPop) {
    Heap heap;
    EXPECT_THROW(heap.heap_pop(), std::out_of_range);
}

TEST(HeapTest, HeapResize) {
    Heap heap(2);
    HeapNode task1{10, 50, 0};
    HeapNode task2{20, 80, 1};
    HeapNode task3{30, 90, 2};

    heap.heap_push(&task1);
    heap.heap_push(&task2);
    heap.heap_push(&task3);

    EXPECT_EQ(heap.heap_getcap(), 4);
    EXPECT_EQ(heap.heap_getsz(), 3);
    EXPECT_EQ(heap.heap_pop().id, 30);
    EXPECT_EQ(heap.heap_pop().id, 20);
    EXPECT_EQ(heap.heap_pop().id, 10);
}

TEST(HeapTest, NullPointer) {
    Heap heap;
    EXPECT_THROW(heap.heap_push(nullptr), std::invalid_argument);
}

TEST(HeapTest, SiftDown) {
    Heap heap;
    HeapNode task1{1, 100, 0};
    HeapNode task2{2, 90, 1};
    HeapNode task3{3, 80, 2};
    HeapNode task4{4, 70, 3};
    HeapNode task5{5, 60, 4};
    HeapNode task6{6, 50, 5};
    HeapNode task7{7, 40, 6};

    heap.heap_push(&task1);
    heap.heap_push(&task2);
    heap.heap_push(&task3);
    heap.heap_push(&task4);
    heap.heap_push(&task5);
    heap.heap_push(&task6);
    heap.heap_push(&task7);

    EXPECT_EQ(heap.heap_getsz(), 7);
    EXPECT_EQ(heap.heap_pop().id, 1);
    EXPECT_EQ(heap.heap_pop().id, 2);
    EXPECT_EQ(heap.heap_pop().id, 3);
    EXPECT_EQ(heap.heap_pop().id, 4);
    EXPECT_EQ(heap.heap_pop().id, 5);
    EXPECT_EQ(heap.heap_pop().id, 6);
    EXPECT_EQ(heap.heap_pop().id, 7);
    EXPECT_EQ(heap.heap_getsz(), 0);
}

TEST(HeapTest, SamePriorityOrder) {
    Heap heap;
    HeapNode task1{10, 80, 0};
    HeapNode task2{20, 80, 2};
    HeapNode task3{30, 80, 1};

    heap.heap_push(&task2);
    heap.heap_push(&task3);
    heap.heap_push(&task1);

    EXPECT_EQ(heap.heap_pop().id, 10);
    EXPECT_EQ(heap.heap_pop().id, 30);
    EXPECT_EQ(heap.heap_pop().id, 20);
}

TEST(HeapTest, SingleElement) {
    Heap heap;
    HeapNode task{10, 80, 0};

    heap.heap_push(&task);

    EXPECT_EQ(heap.heap_getsz(), 1);
    EXPECT_EQ(heap.heap_peek().id, 10);
    EXPECT_EQ(heap.heap_pop().id, 10);
    EXPECT_EQ(heap.heap_getsz(), 0);
    EXPECT_THROW(heap.heap_pop(), std::out_of_range);
}

TEST(HeapTest, PeekAfterPop) {
    Heap heap;
    HeapNode task1{10, 50, 0};
    HeapNode task2{20, 100, 1};
    HeapNode task3{30, 70, 2};

    heap.heap_push(&task1);
    heap.heap_push(&task2);
    heap.heap_push(&task3);

    EXPECT_EQ(heap.heap_peek().id, 20);
    EXPECT_EQ(heap.heap_pop().id, 20);
    EXPECT_EQ(heap.heap_peek().id, 30);
    EXPECT_EQ(heap.heap_getsz(), 2);
}

TEST(HeapTest, RightChildSiftDown) {
    Heap heap;
    HeapNode task1{1, 100, 0};
    HeapNode task2{2, 70, 1};
    HeapNode task3{3, 90, 2};
    HeapNode task4{4, 40, 3};
    HeapNode task5{5, 60, 4};
    HeapNode task6{6, 80, 5};
    HeapNode task7{7, 10, 6};

    heap.heap_push(&task1);
    heap.heap_push(&task2);
    heap.heap_push(&task3);
    heap.heap_push(&task4);
    heap.heap_push(&task5);
    heap.heap_push(&task6);
    heap.heap_push(&task7);

    EXPECT_EQ(heap.heap_pop().id, 1);
    EXPECT_EQ(heap.heap_pop().id, 3);
    EXPECT_EQ(heap.heap_pop().id, 6);
    EXPECT_EQ(heap.heap_pop().id, 2);
    EXPECT_EQ(heap.heap_pop().id, 5);
    EXPECT_EQ(heap.heap_pop().id, 4);
    EXPECT_EQ(heap.heap_pop().id, 7);
    EXPECT_EQ(heap.heap_getsz(), 0);
}

TEST(HeapTest, MixedOperations) {
    Heap heap;
    HeapNode task1{10, 40, 0};
    HeapNode task2{20, 80, 1};
    HeapNode task3{30, 60, 2};
    HeapNode task4{40, 90, 3};
    HeapNode task5{50, 70, 4};

    heap.heap_push(&task1);
    heap.heap_push(&task2);
    heap.heap_push(&task3);

    EXPECT_EQ(heap.heap_pop().id, 20);

    heap.heap_push(&task4);
    heap.heap_push(&task5);

    EXPECT_EQ(heap.heap_getsz(), 4);
    EXPECT_EQ(heap.heap_pop().id, 40);
    EXPECT_EQ(heap.heap_pop().id, 50);
    EXPECT_EQ(heap.heap_pop().id, 30);
    EXPECT_EQ(heap.heap_pop().id, 10);
    EXPECT_EQ(heap.heap_getsz(), 0);
}