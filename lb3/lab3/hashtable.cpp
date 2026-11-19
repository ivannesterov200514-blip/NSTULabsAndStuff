#include "hashtable.h"
#include <algorithm>
#include <sstream>

int hash1(string key, int m) {
    if (m <= 0) return 0;
    long long h = 0;
    for (size_t i = 0; i < key.size(); i++) {
        h = (h * 31 + (unsigned char)key[i]) % m;
    }
    return (int)h;
}

int hash2(string key, int m) {
    if (m <= 0) return 0;
    long long h = 7;
    for (size_t i = 0; i < key.size(); i++) {
        h = (h * 131 + (unsigned char)key[i]) % m;
    }
    return (int)h;
}

ChainTable::ChainTable(int c) : cap(c > 0 ? c : 101), sz(0) {
    buckets = new UList[cap];
}

void ChainTable::ct_put(string key, string value) {
    int i = hash1(key, cap);
    if (buckets[i].ul_update_value(key, value)) return;
    buckets[i].ul_insert_head(key, value);
    sz++;
}

bool ChainTable::ct_get(string key, string& value) const {
    int i = hash1(key, cap);
    return buckets[i].ul_get_value(key, value);
}

bool ChainTable::ct_remove(string key) {
    int i = hash1(key, cap);
    if (!buckets[i].ul_remove_key(key)) return false;
    sz--;
    return true;
}

void ChainTable::ct_print() const {
    if (sz == 0) {
        cout << "Хеш-таблица пуста\n";
        return;
    }
    cout << "Хеш-таблица (цепочки):\n";
    for (int i = 0; i < cap; i++) {
        if (buckets[i].ul_getsz() == 0) continue;
        cout << "[" << i << "] ";
        buckets[i].ul_print_pairs();
    }
}

int ChainTable::ct_getsz() const {
    return sz;
}

int ChainTable::ct_getcap() const {
    return cap;
}

vector<pair<string, string>> ChainTable::ct_get_all() const {
    vector<pair<string, string>> result;
    for (int i = 0; i < cap; i++) {
        vector<pair<string, string>> bucket_items = buckets[i].ul_get_pairs();
        result.insert(result.end(), bucket_items.begin(), bucket_items.end());
    }
    return result;
}

ChainTable::~ChainTable() {
    delete[] buckets;
}

CuckooTable::Slot::Slot() : key(""), value("") {}

CuckooTable::CuckooTable(int c) : cap(c > 0 ? c : 11), sz(0), grows(0), kicks(0) {
    t1 = new Slot[cap];
    t2 = new Slot[cap];
}

void CuckooTable::cu_insert(string key, string value) {
    for (int step = 0; step < 32; step++) {
        int i = hash1(key, cap);
        swap(key, t1[i].key);
        swap(value, t1[i].value);
        if (key.empty()) {
            sz++;
            return;
        }
        kicks++;

        int j = hash2(key, cap);
        swap(key, t2[j].key);
        swap(value, t2[j].value);
        if (key.empty()) {
            sz++;
            return;
        }
        kicks++;
    }
    cu_grow(key, value);
}

void CuckooTable::cu_grow(string key, string value) {
    int old = cap;
    Slot* old_t1 = t1;
    Slot* old_t2 = t2;

    cap = old * 2 + 1;
    t1 = new Slot[cap];
    t2 = new Slot[cap];
    sz = 0;
    grows++;

    for (int i = 0; i < old; i++) {
        if (!old_t1[i].key.empty()) cu_insert(old_t1[i].key, old_t1[i].value);
        if (!old_t2[i].key.empty()) cu_insert(old_t2[i].key, old_t2[i].value);
    }

    delete[] old_t1;
    delete[] old_t2;
    cu_insert(key, value);
}

void CuckooTable::cu_put(string key, string value) {
    if (key.empty()) return;

    Slot* a = &t1[hash1(key, cap)];
    if (a->key == key) {
        a->value = value;
        return;
    }

    Slot* b = &t2[hash2(key, cap)];
    if (b->key == key) {
        b->value = value;
        return;
    }

    cu_insert(key, value);
}

bool CuckooTable::cu_get(string key, string& value) const {
    if (key.empty()) return false;

    const Slot* a = &t1[hash1(key, cap)];
    if (a->key == key) {
        value = a->value;
        return true;
    }

    const Slot* b = &t2[hash2(key, cap)];
    if (b->key == key) {
        value = b->value;
        return true;
    }

    return false;
}

bool CuckooTable::cu_remove(string key) {
    if (key.empty()) return false;

    Slot* a = &t1[hash1(key, cap)];
    if (a->key == key) {
        a->key = "";
        a->value = "";
        sz--;
        return true;
    }

    Slot* b = &t2[hash2(key, cap)];
    if (b->key == key) {
        b->key = "";
        b->value = "";
        sz--;
        return true;
    }

    return false;
}

void CuckooTable::cu_print() const {
    if (sz == 0) {
        cout << "Хеш-таблица пуста\n";
        return;
    }
    cout << "Хеш-таблица (кукушка):\n";
    for (int i = 0; i < cap; i++) {
        if (!t1[i].key.empty()) {
            cout << "T1[" << i << "] " << t1[i].key << "=" << t1[i].value << endl;
        }
    }
    for (int i = 0; i < cap; i++) {
        if (!t2[i].key.empty()) {
            cout << "T2[" << i << "] " << t2[i].key << "=" << t2[i].value << endl;
        }
    }
}

int CuckooTable::cu_getsz() const {
    return sz;
}

int CuckooTable::cu_getcap() const {
    return cap;
}

int CuckooTable::cu_getgrows() const {
    return grows;
}

uint64_t CuckooTable::cu_getkicks() const {
    return kicks;
}

vector<pair<string, string>> CuckooTable::cu_get_all() const {
    vector<pair<string, string>> result;
    for (int i = 0; i < cap; i++) {
        if (!t1[i].key.empty()) result.push_back({t1[i].key, t1[i].value});
    }
    for (int i = 0; i < cap; i++) {
        if (!t2[i].key.empty()) result.push_back({t2[i].key, t2[i].value});
    }
    return result;
}

CuckooTable::~CuckooTable() {
    delete[] t1;
    delete[] t2;
}