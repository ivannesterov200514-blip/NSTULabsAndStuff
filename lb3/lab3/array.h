#pragma once
#include <iostream>
#include <string>

using namespace std;

class Arr {
private:
    string* data;
    int size;
    int capacity;
public:
    Arr(int cap = 2);
    void arr_resize (int newcap);
    string arr_search (int idx);
    void arr_insert_tail (string val);
    void arr_insert_idx (string val, int idx);
    void arr_delete_idx (int idx);
    void arr_change_idx (string val, int idx);
    int arr_getsz();
    void arr_print();
    ~Arr();
};