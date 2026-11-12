#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct KVNode {
    string key;
    string value;
    KVNode* next;
    KVNode(string k, string v, KVNode* t) : key(k), value(v), next(t) {}
};

int hash1 (string key, int m);
int hash2 (string key, int m);

struct ChainTable {
    KVNode** buckets;
    int cap;
    int sz;
    ChainTable(int c = 101) : cap(c), sz(0) {
        buckets = new KVNode*[cap];
        for (int i = 0; i < cap; i++) buckets[i] = nullptr;
    }
    ~ChainTable() {
        for (int i = 0; i < cap; i++) {
            while (buckets[i] != nullptr) {
                KVNode* v = buckets[i];
                buckets[i] = v->next;
                delete v;
            }
        }
        delete[] buckets;
    }
};

void ct_put (ChainTable* t, string key, string value);
bool ct_get (ChainTable* t, string key, string& value);
bool ct_remove (ChainTable* t, string key);
void ct_print (ChainTable* t);

struct Slot {
    string key;
    string value;
};

struct CuckooTable {
    Slot* t1;
    Slot* t2;
    int cap;
    int sz;
    int grows;
    uint64_t kicks;
    CuckooTable(int c = 11) : cap(c), sz(0), grows(0), kicks(0) {
        t1 = new Slot[cap];
        t2 = new Slot[cap];
    }
    ~CuckooTable() {
        delete[] t1;
        delete[] t2;
    }
};

void cu_put (CuckooTable* t, string key, string value);
bool cu_get (CuckooTable* t, string key, string& value);
bool cu_remove (CuckooTable* t, string key);
void cu_print (CuckooTable* t);

bool genealogy (const vector<string>& t);
