#pragma once
#include "dinarray.h"
#include "table.h"
#include "heap.h"

struct TaskStats {
    int total = 0;
    int queued = 0;
    int running = 0;
    int completed = 0;
    int failed = 0;
    int cancelled = 0;
    int queue_size = 0;
    int max_priority = -1;
};

class TaskManager {
private:
    Arr tasks;
    HashTable table;
    Heap heap;
    uint64_t next_id;
    uint64_t next_order;
public:
    TaskManager();
    uint64_t createTask(const string& type, const string& payload, unsigned int priority);
    Task* getTask(uint64_t id);
    Task* startNextTask();
    void completeTask(uint64_t id);
    void failTask(uint64_t id, const string& error);
    void cancelTask(uint64_t id);
    void listTasks(Arr& result, const string& status = "");
    TaskStats getStats();
    void saveToFile(const string& path);
    void loadFromFile(const string& path);
    void swap(TaskManager& other) noexcept;
    int getHashResizeCount();
    double getHashMaxLoadFactor();
};