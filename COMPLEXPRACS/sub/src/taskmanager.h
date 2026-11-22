#pragma once
#include "dinarray.h"
#include "table.h"
#include "heap.h"

class TaskManager {
private:
    Arr tasks;
    HashTable table;
    Heap heap;
    uint64_t next_id;
    uint64_t next_order;
public:
    TaskManager();
    uint64_t createTask(string type, string payload, unsigned int priority);

};