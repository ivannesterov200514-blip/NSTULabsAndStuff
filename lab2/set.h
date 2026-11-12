#pragma once
#include <iostream>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "ulist.h"

using namespace std;

struct Set {
    UList* table;
    int cap;
    int sz;
    Set(int c = 4099) : cap(c), sz(0) {
        table = new UList[cap];
    }
    ~Set() { delete[] table; }
};

void set_add (Set* s, string val);
void set_del (Set* s, string val);
bool set_at (Set* s, string val);
void set_print (Set* s);

bool turtles (const vector<string>& t, int& answer);