#include "array.h"
#include <stdexcept>

Arr::Arr(int cap) {
    capacity = cap > 0 ? cap : 2;
    data = new Task[capacity];
    size = 0;
}

int Arr::arr_getsz(){
    return size;
}

void Arr::arr_resize (int newcap) {
    if (newcap <= 0 || newcap < size) {
        throw invalid_argument("resize(arr): new capacity is smaller than size");
    }
    Task* newdata = new Task[newcap];
    for (int i = 0; i < size; i++) newdata[i] = data[i];
    delete[] data;
    data = newdata;
    capacity = newcap;
}

Task* Arr::arr_search (int idx) {
    if (idx >= size || idx < 0) {
        throw out_of_range("Такого индекса нет");
    }
    return &data[idx];
}

void Arr::arr_insert_tail (Task* val) {
    if (val == nullptr) {
        throw invalid_argument("Пустой указатель");
    }

    if (size >= capacity) arr_resize(capacity*2);
    data[size] = *val;
    size++;
}

void Arr::arr_delete_idx (int idx){
    if (idx >= size || idx < 0) {
        throw out_of_range("Такого индекса нет");
    }
    for (int i = idx; i < size - 1; ++i) {
        data[i] = data[i+1];
    }
    size--;
}

void Arr::arr_change_idx (Task* val, int idx){
    if (val == nullptr) {
        throw invalid_argument("Пустой указатель");
    }

    if (idx >= size || idx < 0) {
        throw out_of_range("Такого индекса нет");
    }
    data[idx] = *val;
}

void Arr::arr_print(){
    cout << "Массив: ";
    for (int i = 0; i < size; ++i) {
        cout << data[i].id << " ";
    }
    cout << endl;
}

Arr::~Arr() { delete[] data; }
