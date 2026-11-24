#include "taskmanager.h"
#include <ctime>
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <utility>

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
    HashNode* node = table.hash_search(id);
    if (node == nullptr) {
        throw out_of_range("Задача с таким ID не найдена");
    }
    return tasks.arr_search(node->index);
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
    string newError = error;

    task->updated_at.swap(now);
    task->error.swap(newError);
    task->status = "failed";
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

void TaskManager::saveToFile(const string& path) {
    ofstream file(path + ".tmp", ios::binary);
    
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл");
    }
    file << "TASKS_V1\n";
    file << next_id << " " << next_order << "\n";
    file << tasks.arr_getsz() << "\n";

    for (int i = 0; i < tasks.arr_getsz(); i++) {
        Task* task = tasks.arr_search(i);
        file << task->id << " ";
        file << task->priority << " ";
        file << quoted(task->type) << " ";
        file << quoted(task->payload) << " ";
        file << task->status << " ";
        file << quoted(task->created_at) << " ";
        file << quoted(task->updated_at) << " ";
        file << quoted(task->error) << "\n";
    }

    file.flush();
    if (!file) {
        throw runtime_error("Ошибка записи файла");
    }
    file.close();
    if (!file) {
        throw runtime_error("Ошибка закрытия файла");
    }

    filesystem::rename(path + ".tmp", path);
}

void TaskManager::loadFromFile(const string& path) {
    if (!filesystem::exists(path)) return;
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл");
    }

    string version;
    file >> version;

    if (!file || version != "TASKS_V1") {
        throw runtime_error("Неверная версия или повреждённый файл");
    }
    uint64_t fileNextId;
    uint64_t fileNextOrder;
    int count;

    file >> fileNextId >> fileNextOrder >> count;
    if (!file || count < 0) {
        throw runtime_error("Повреждённый файл");
    }
    TaskManager loaded;
    uint64_t maxId = 0;

    for (int i = 0; i < count; i++) {
        Task task{};
        file >> task.id;
        file >> task.priority;
        file >> quoted(task.type);
        file >> quoted(task.payload);
        file >> task.status;
        file >> quoted(task.created_at);
        file >> quoted(task.updated_at);
        file >> quoted(task.error);

        if (!file) {
            throw runtime_error("Повреждённая запись задачи");
        }
        if (task.id == 0 || task.priority > 100) {
            throw runtime_error("Некорректные данные задачи");
        }
        if (task.status != "queued" && task.status != "running" &&
            task.status != "completed" && task.status != "failed" && task.status != "cancelled") {
            throw runtime_error("Некорректный статус задачи");
        }

        if (loaded.table.hash_contains(task.id)) {
            throw runtime_error("Повторяющийся ID задачи");
        }
        if (task.id > maxId) {
            maxId = task.id;
        }
        if (task.id != static_cast<uint64_t>(i) + 1) {
            throw runtime_error("Нарушен порядок ID задач");
        }

        loaded.tasks.arr_insert_tail(&task);
        int idx = loaded.tasks.arr_getsz() - 1;
        loaded.table.hash_insert(task.id, idx);

        if (task.status == "queued") {
            HeapNode node{task.id, task.priority, static_cast<uint64_t>(i)};
            loaded.heap.heap_push(&node);
        }
    }
    if (fileNextId == 0 || fileNextId <= maxId) {
        throw runtime_error("Некорректный следующий ID");
    }

    if (fileNextOrder < static_cast<uint64_t>(count)) {
        throw runtime_error("Некорректный порядок создания задач");
    }

    string extra;
    if (file >> extra) {
        throw runtime_error("Лишние данные в файле");
    }
    if (!file.eof()) {
        throw runtime_error("Ошибка чтения файла");
    }
    loaded.next_id = fileNextId;
    loaded.next_order = fileNextOrder;
    swap(loaded);
}

void TaskManager::swap(TaskManager& other) noexcept {
    tasks.swap(other.tasks);
    table.swap(other.table);
    heap.swap(other.heap);
    std::swap(next_id, other.next_id);
    std::swap(next_order, other.next_order);
}