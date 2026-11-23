#include "table.h"
#include <utility>

HashTable::HashTable(int cap) {
    capacity = cap > 0 ? cap : 2;
    table = new HashNode*[capacity];
    for (int i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }
    size = 0;
    resize_count = 0;
    max_load_factor = 0.0;
}

// splitmix64 
uint64_t HashTable::hash(uint64_t id) {
    uint64_t result = id + 0x9e3779b97f4a7c15ULL;
    result = (result ^ (result >> 30)) * 0xbf58476d1ce4e5b9ULL;
    result = (result ^ (result >> 27)) * 0x94d049bb133111ebULL;
    result = result ^ (result >> 31);
    return result;    
}


void HashTable::hash_insert(uint64_t id, int idx) {
    if (idx < 0) {
        throw invalid_argument("Неверный индекс");
    }

    uint64_t cell = hash(id) % capacity;
    HashNode* current = table[cell];

    while (current != nullptr) {
        if (current->id == id) {
            throw invalid_argument("Такой ID уже существует");
        }
        current = current->next;
    }

    HashNode* node = new HashNode;
    node->id = id;
    node->index = idx;
    node->next = table[cell];
    table[cell] = node;
    size++;
    
    double load_factor = static_cast<double>(size) / capacity;

    if (load_factor > max_load_factor) {
        max_load_factor = load_factor;
    }

    if (load_factor >= 0.75) {
        hash_resize();
    }
}

HashNode* HashTable::hash_search(uint64_t id) {
    uint64_t cell = hash(id) % capacity;
    HashNode* current = table[cell];

    while (current != nullptr) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }

    throw out_of_range("Такой ID не найден");
}

void HashTable::hash_delete(uint64_t id) {
    uint64_t cell = hash(id) % capacity;
    HashNode* previous = nullptr;
    HashNode* current = table[cell];

    while (current != nullptr) {
        if (current->id == id) {
            if (previous == nullptr) {
                table[cell] = current->next;
            }
            else {
                previous->next = current->next;
            }
            delete current;
            size--;
            return;
        }

        previous = current;
        current = current->next;
    }

    throw out_of_range("Такой ID не найден");
}

bool HashTable::hash_contains(uint64_t id) {
    uint64_t cell = hash(id) % capacity;
    HashNode* current = table[cell];

    while (current != nullptr) {
        if (current->id == id) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void HashTable::hash_change(uint64_t id, int idx) {
    if (idx < 0) {
        throw invalid_argument("Неверный индекс");
    }

    HashNode* found = hash_search(id);
    found->index = idx;
}


void HashTable::hash_resize() {
    HashNode** prevtable = table;
    int oldcapacity = capacity;
    int newcapacity = capacity * 2;

    HashNode** newtable = new HashNode*[newcapacity];

    for (int i = 0; i < newcapacity; i++) {
        newtable[i] = nullptr;
    }
    capacity = newcapacity;
    table = newtable;

    for (int i = 0; i < oldcapacity; i++) {
        HashNode* current = prevtable[i];

        while (current != nullptr) {
            HashNode* next = current->next;
            uint64_t cell = hash(current->id) % capacity;
            current->next = table[cell];
            table[cell] = current;
            current = next;
        }
    }
    delete[] prevtable;
    resize_count++;
}

int HashTable::hash_get_resize_count() {
    return resize_count;
}

double HashTable::hash_get_max_load_factor() {
    return max_load_factor;
}

void HashTable::swap(HashTable& other) noexcept {
    std::swap(table, other.table);
    std::swap(capacity, other.capacity);
    std::swap(size, other.size);
    std::swap(resize_count, other.resize_count);
    std::swap(max_load_factor, other.max_load_factor);
}

HashTable::~HashTable() {
    for (int i = 0; i < capacity; i++) {
        HashNode* current = table[i];
        while (current != nullptr) {
            HashNode* next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] table;
}