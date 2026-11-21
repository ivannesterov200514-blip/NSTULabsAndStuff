#include <gtest/gtest.h>
#include <stdexcept>
#include "table.h"

using namespace std;

TEST(HashtableTest, EmptyTable) {
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