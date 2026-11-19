#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "ulist.h"

using namespace std;

int hash1(string key, int m);
int hash2(string key, int m);

class ChainTable {
private:
    UList* buckets;
    int cap;
    int sz;

public:
    ChainTable(int c = 101);
    ChainTable(const ChainTable& other) = delete;
    ChainTable& operator=(const ChainTable& other) = delete;

    void ct_put(string key, string value);
    bool ct_get(string key, string& value) const;
    bool ct_remove(string key);
    void ct_print() const;

    int ct_getsz() const;
    int ct_getcap() const;
    vector<pair<string, string>> ct_get_all() const;

    ~ChainTable();
};

class CuckooTable {
private:
    class Slot {
    private:
        string key;
        string value;

        friend class CuckooTable;
    public:
        Slot();
    };

    Slot* t1;
    Slot* t2;
    int cap;
    int sz;
    int grows;
    uint64_t kicks;

    void cu_insert(string key, string value);
    void cu_grow(string key, string value);

public:
    CuckooTable(int c = 11);
    CuckooTable(const CuckooTable& other) = delete;
    CuckooTable& operator=(const CuckooTable& other) = delete;

    void cu_put(string key, string value);
    bool cu_get(string key, string& value) const;
    bool cu_remove(string key);
    void cu_print() const;

    int cu_getsz() const;
    int cu_getcap() const;
    int cu_getgrows() const;
    uint64_t cu_getkicks() const;
    vector<pair<string, string>> cu_get_all() const;

    ~CuckooTable();
};
