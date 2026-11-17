#pragma once
#include "tasks.h"
#include <iostream>
#include <string>

using namespace std;

class Arr {
private:
    Task* data;
    int size;
    int capacity;
public:
    Arr(int cap);

    void arr_resize (int newcap);
    Task* arr_search (int idx);
    void arr_insert_tail (Task* val);
    void arr_insert_idx (Task* val, int idx);
    void arr_delete_idx (int idx);
    void arr_change_idx (Task* val, int idx);
    int arr_getsz();
    void arr_print();
    ~Arr();
};