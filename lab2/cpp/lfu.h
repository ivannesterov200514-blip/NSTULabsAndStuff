#pragma once
#include <cstdint>
#include <iostream>
#include <string>

#include "hashtable.h"

using namespace std;

struct Lfu {
    int cap;
    uint64_t clock;
    ChainTable* data;
    Lfu(int c = 2) : cap(c), clock(0) {
        data = new ChainTable(101);
    }
    ~Lfu() { delete data; }
};

void lfu_put (Lfu* c, string key, string value, uint64_t freq, uint64_t stamp);
void lfu_set (Lfu* c, string key, string value);
bool lfu_get (Lfu* c, string key, string& value);
void lfu_print (Lfu* c);
