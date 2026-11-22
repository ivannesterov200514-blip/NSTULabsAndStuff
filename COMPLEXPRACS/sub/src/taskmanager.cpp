#include "taskmanager.h"

TaskManager::TaskManager() {
    next_id = 1;
    next_order = 0;
}

uint64_t TaskManager::createTask(string type, string payload, unsigned int priority) {
    if (priority > 100) {
        throw invalid_argument("Приоритет должен быть от 0 до 100");
    }

    Task newtask {next_id, type, payload, priority, "queued"};
    tasks.arr_insert_tail(&newtask);
    table.hash_insert(newtask.id, tasks.arr_getsz() - 1);

    HeapNode newnode {newtask.id, newtask.priority, next_order};
    heap.heap_push(&newnode);

    next_id++;
    next_order++;
    return newtask.id;
}