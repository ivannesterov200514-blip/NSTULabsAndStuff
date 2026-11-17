#pragma once
#include "array.h"
#include <iostream>
#include <stdint.h>

using namespace std;

struct HashNode {
    uint64_t id;
    int index;
    HashNode* next;
};

class HashTable {
private:
    HashNode** table;
    int capacity;
    int size;
public:
    HashTable(int cap = 2);
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    uint64_t hash(uint64_t id);

    void hash_insert(uint64_t id, int idx);
    HashNode* hash_search(uint64_t id);
    void hash_delete(uint64_t id);
    bool hash_contains(uint64_t id);
    void hash_change(uint64_t id, int idx);
    void hash_resize();
    ~HashTable();
};