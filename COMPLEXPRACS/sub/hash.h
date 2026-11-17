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

    uint64_t hash(uint64_t id);

    void hash_insert(Task* val);
    Task* hash_search(int id);
    void hash_delete(int id);
    void hash_print();
    ~HashTable();
};