#include "hash.h"

HashTable::HashTable(int cap = 2) {
    capacity = cap > 0 ? cap : 2;
    table = new HashNode*[capacity];
    size = 0;
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