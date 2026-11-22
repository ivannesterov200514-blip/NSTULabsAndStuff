#include "taskmanager.h"
#include <ctime>
#include <stdexcept>

TaskManager::TaskManager() {
    next_id = 1;
    next_order = 0;
}

string get_current_time() {
    time_t now = time(nullptr);
    tm* current = localtime(&now);

    if (current == nullptr) {
        throw runtime_error("Ошибка получения времени");
    }

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", current);

    return string(buffer);
}

uint64_t TaskManager::createTask(const string& type, const string& payload, unsigned int priority) {
    if (priority > 100) {
        throw invalid_argument("Приоритет должен быть от 0 до 100");
    }

    string now = get_current_time();
    Task newtask{next_id, type, payload, priority, "queued", now, now};
    HeapNode newnode{newtask.id, newtask.priority, next_order};

    int idx = tasks.arr_getsz();
    bool arrayAdded = false;

    try {
        tasks.arr_insert_tail(&newtask);
        arrayAdded = true;
        table.hash_insert(newtask.id, idx);
        heap.heap_push(&newnode);
    }
    catch (...) {
        if (arrayAdded) {
            if (table.hash_contains(newtask.id)) {
                table.hash_delete(newtask.id);
            }
            tasks.arr_delete_idx(idx);
        }
        throw;
    }
    next_id++;
    next_order++;
    return newtask.id;
}

Task* TaskManager::getTask(uint64_t id) {
    int idx = table.hash_search(id)->index;
    return tasks.arr_search(idx);
}

Task* TaskManager::startNextTask() {
    while (heap.heap_getsz() > 0) {
        HeapNode node = heap.heap_peek();
        Task* task = getTask(node.id);

        if (task->status != "queued") {
            heap.heap_pop();
            continue;
        }

        string now = get_current_time();

        heap.heap_pop();
        task->status = "running";
        task->updated_at = now;
        return task;
    }
    throw out_of_range("Нет задач для запуска");
}

void TaskManager::completeTask(uint64_t id) {
    Task* task = getTask(id);

    if (task->status != "running") {
        throw logic_error("Задача не выполняется");
    }

    string now = get_current_time();

    task->status = "completed";
    task->updated_at = now;
}

void TaskManager::failTask(uint64_t id, const string& error) {
    Task* task = getTask(id);

    if (task->status != "running") {
        throw logic_error("Задача не выполняется");
    }

    string now = get_current_time();
    task->status = "failed";
    task->updated_at = now;
    task->error = error;
}

void TaskManager::cancelTask(uint64_t id) {
    Task* task = getTask(id);

    if (task->status != "queued") {
        throw logic_error("Задачу нельзя отменить");
    }

    string now = get_current_time();

    task->status = "cancelled";
    task->updated_at = now;
}

TaskStats TaskManager::getStats() {
    TaskStats stats;
    stats.total = tasks.arr_getsz();

    for (int i = 0; i < stats.total; i++) {
        Task* task = tasks.arr_search(i);

        if (task->status == "queued") {
            stats.queued++;
            stats.queue_size++;
            if (static_cast<int>(task->priority) > stats.max_priority) {
                stats.max_priority = static_cast<int>(task->priority);
            }
        }
        if (task->status == "completed") stats.completed++;
        if (task->status == "running") stats.running++;
        if (task->status == "cancelled") stats.cancelled++;
        if (task->status == "failed") stats.failed++;
    }
    return stats;
}

void TaskManager::listTasks(Arr& result, const string& status) {
    for (int i = 0; i < tasks.arr_getsz(); i++) {
        Task* task = tasks.arr_search(i);
        if (status == "" || task->status == status) {
            result.arr_insert_tail(task);
        }
    }
}