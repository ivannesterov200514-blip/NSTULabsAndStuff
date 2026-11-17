#pragma once
#include "array.h"

#include <iostream>
using namespace std;

struct HashNode {
    Task* task;
    HashNode* next;
};

int hash(int id) {

}

class HashTable {
private:
    HashNode** table;
    int capacity;
    int size;
public:
    HashTable(int cap = 2);

    void hash_insert(Task* val);
    Task* hash_search(int id);
    void hash_delete(int id);
    void hash_print();
    ~HashTable();
};