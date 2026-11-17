#include "hash.h"

HashTable::HashTable(int cap) {
    capacity = cap > 0 ? cap : 2;
    table = new HashNode*[capacity];
    for (int i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }
    size = 0;
}

// splitmix64 
uint64_t HashTable::hash(uint64_t id) {
    uint64_t result;
    result = (result ^ (result >> 30)) * 0xbf58476d1ce4e5b9ULL;
    result = (result ^ (result >> 27)) * 0x94d049bb133111ebULL;
    result = result ^ (result >> 31);
    return result;    
}

void HashTable::hash_insert(Task* val) {
    
}

Task* HashTable::hash_search(int id) {

}

void HashTable::hash_delete(int id) {
    
}

void HashTable::hash_print() {
    
}

HashTable::~HashTable() {
    delete[] table;
}