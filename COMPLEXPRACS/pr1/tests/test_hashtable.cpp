#include <gtest/gtest.h>
#include <stdexcept>
#include <cstdint>
#include "table.h"

TEST(HashTest, EmptyTable) {
    HashTable table;

    EXPECT_FALSE(table.hash_contains(10));
}

TEST(HashTest, InsertAndSearch) {
    HashTable table;

    table.hash_insert(10, 0);

    EXPECT_TRUE(table.hash_contains(10));
    EXPECT_EQ(table.hash_search(10)->id, 10);
    EXPECT_EQ(table.hash_search(10)->index, 0);
}

TEST(HashTest, DuplicateID) {
    HashTable table;

    table.hash_insert(10, 0);

    EXPECT_THROW(table.hash_insert(10, 1), std::invalid_argument);
    EXPECT_EQ(table.hash_search(10)->index, 0);
}

TEST(HashTest, ChangeElement) {
    HashTable table;

    table.hash_insert(10, 0);
    table.hash_change(10, 5);

    EXPECT_EQ(table.hash_search(10)->index, 5);
    EXPECT_EQ(table.hash_search(10)->id, 10ULL);
    EXPECT_THROW(table.hash_change(10, -1), std::invalid_argument);
    EXPECT_EQ(table.hash_search(10)->index, 5);
}

TEST(HashTest, DeleteElement) {
    HashTable table;

    table.hash_insert(10, 0);
    table.hash_insert(20, 1);
    table.hash_delete(10);

    EXPECT_FALSE(table.hash_contains(10));
    EXPECT_TRUE(table.hash_contains(20));
    EXPECT_EQ(table.hash_search(20)->index, 1);
}

TEST(HashTest, InvalidSearch) {
    HashTable table;

    EXPECT_THROW(table.hash_search(10), std::out_of_range);
}

TEST(HashTest, InvalidChange) {
    HashTable table;

    EXPECT_THROW(table.hash_change(10, 1), std::out_of_range);
}

TEST(HashTest, InvalidDelete) {
    HashTable table;

    EXPECT_THROW(table.hash_delete(10), std::out_of_range);
}

TEST(HashTest, Collision) {
    HashTable table(100);
    uint64_t id1 = 10;
    uint64_t id2 = 11;

    while (table.hash(id2) % 100 != table.hash(id1) % 100) {
        id2++;
    }

    table.hash_insert(id1, 0);
    table.hash_insert(id2, 1);

    EXPECT_TRUE(table.hash_contains(id1));
    EXPECT_TRUE(table.hash_contains(id2));
    EXPECT_EQ(table.hash_search(id1)->index, 0);
    EXPECT_EQ(table.hash_search(id2)->index, 1);
}

TEST(HashTest, DeleteCollision) {
    HashTable table(100);
    uint64_t id1 = 10;
    uint64_t id2 = 11;
    uint64_t id3 = 12;

    while (table.hash(id2) % 100 != table.hash(id1) % 100) {
        id2++;
    }
    id3 = id2 + 1;
    while (table.hash(id3) % 100 != table.hash(id1) % 100) {
        id3++;
    }

    table.hash_insert(id1, 0);
    table.hash_insert(id2, 1);
    table.hash_insert(id3, 2);
    table.hash_delete(id2);

    EXPECT_FALSE(table.hash_contains(id2));
    EXPECT_EQ(table.hash_search(id1)->index, 0);
    EXPECT_EQ(table.hash_search(id3)->index, 2);
}

TEST(HashTest, HashResize) {
    HashTable table(2);

    EXPECT_EQ(table.hash_get_resize_count(), 0);
    table.hash_insert(10, 0);
    EXPECT_EQ(table.hash_get_resize_count(), 0);
    table.hash_insert(20, 1);

    EXPECT_EQ(table.hash_get_resize_count(), 1);
    EXPECT_EQ(table.hash_search(10)->index, 0);
    EXPECT_EQ(table.hash_search(20)->index, 1);
}

TEST(HashTest, ResizeDataIntegrity) {
    HashTable table(2);

    for (int i = 0; i < 20; i++) {
        table.hash_insert(i + 1, i);
    }

    EXPECT_GT(table.hash_get_resize_count(), 0);
    for (int i = 0; i < 20; i++) {
        EXPECT_EQ(table.hash_search(i + 1)->index, i);
    }
}

TEST(HashTest, ResizeStatistics) {
    HashTable table(4);

    EXPECT_EQ(table.hash_get_resize_count(), 0);
    EXPECT_DOUBLE_EQ(table.hash_get_max_load_factor(), 0.0);

    table.hash_insert(10, 0);
    table.hash_insert(20, 1);
    table.hash_insert(30, 2);

    EXPECT_EQ(table.hash_get_resize_count(), 1);
    EXPECT_DOUBLE_EQ(table.hash_get_max_load_factor(), 0.75);
}