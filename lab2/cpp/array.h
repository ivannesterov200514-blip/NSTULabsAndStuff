#pragma once
#include <iostream>
#include <string>

using namespace std;

struct Arr {
    string* data;
    int size;
    int capacity;
    Arr(int cap = 2) {
        capacity = cap > 0 ? cap : 2;
        data = new string[capacity];
        size = 0;
    }
    ~Arr() { delete[] data; }
};

string arr_search (Arr* arr, int idx);
void arr_insert_tail (Arr* arr, string val);
void gistogram (Arr* arr);
