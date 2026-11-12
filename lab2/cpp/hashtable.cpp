#include "hashtable.h"
#include <algorithm>
#include <sstream>

int hash1 (string key, int m) {
    long long h = 0;
    for (size_t i = 0; i < key.size(); i++) {
        h = (h * 31 + (unsigned char)key[i]) % m;
    }
    return (int)h;
}

int hash2 (string key, int m) {
    long long h = 7;
    for (size_t i = 0; i < key.size(); i++) {
        h = (h * 131 + (unsigned char)key[i]) % m;
    }
    return (int)h;
}

void ct_put (ChainTable* t, string key, string value) {
    int i = hash1(key, t->cap);
    for (KVNode* cur = t->buckets[i]; cur != nullptr; cur = cur->next) {
        if (cur->key == key) {
            cur->value = value;
            return;
        }
    }
    t->buckets[i] = new KVNode(key, value, t->buckets[i]);
    t->sz++;
}

bool ct_get (ChainTable* t, string key, string& value) {
    for (KVNode* cur = t->buckets[hash1(key, t->cap)]; cur != nullptr; cur = cur->next) {
        if (cur->key == key) {
            value = cur->value;
            return true;
        }
    }
    return false;
}

bool ct_remove (ChainTable* t, string key) {
    int i = hash1(key, t->cap);
    KVNode* prev = nullptr;
    KVNode* cur = t->buckets[i];
    while (cur != nullptr && cur->key != key) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == nullptr) return false;
    if (prev == nullptr) t->buckets[i] = cur->next;
    else prev->next = cur->next;
    delete cur;
    t->sz--;
    return true;
}

void ct_print (ChainTable* t) {
    if (t->sz == 0) {
        cout << "Хеш-таблица пуста\n";
        return;
    }
    cout << "Хеш-таблица (цепочки):\n";
    for (int i = 0; i < t->cap; i++) {
        if (t->buckets[i] == nullptr) continue;
        cout << "[" << i << "] ";
        for (KVNode* cur = t->buckets[i]; cur != nullptr; cur = cur->next) {
            cout << cur->key << "=" << cur->value << " -> ";
        }
        cout << "nullptr\n";
    }
}

static void cu_grow (CuckooTable* t, string key, string value);

static void cu_insert (CuckooTable* t, string key, string value) {
    for (int step = 0; step < 32; step++) {
        int i = hash1(key, t->cap);
        swap(key, t->t1[i].key);
        swap(value, t->t1[i].value);
        if (key.empty()) {
            t->sz++;
            return;
        }
        t->kicks++;

        int j = hash2(key, t->cap);
        swap(key, t->t2[j].key);
        swap(value, t->t2[j].value);
        if (key.empty()) {
            t->sz++;
            return;
        }
        t->kicks++;
    }
    cu_grow(t, key, value);
}

static void cu_grow (CuckooTable* t, string key, string value) {
    int old = t->cap;
    Slot* o1 = t->t1;
    Slot* o2 = t->t2;
    t->cap = old * 2 + 1;
    t->t1 = new Slot[t->cap];
    t->t2 = new Slot[t->cap];
    t->sz = 0;
    t->grows++;
    for (int i = 0; i < old; i++) {
        if (!o1[i].key.empty()) cu_insert(t, o1[i].key, o1[i].value);
        if (!o2[i].key.empty()) cu_insert(t, o2[i].key, o2[i].value);
    }
    delete[] o1;
    delete[] o2;
    cu_insert(t, key, value);
}

void cu_put (CuckooTable* t, string key, string value) {
    if (key.empty()) return;
    Slot* a = &t->t1[hash1(key, t->cap)];
    if (a->key == key) {
        a->value = value;
        return;
    }
    Slot* b = &t->t2[hash2(key, t->cap)];
    if (b->key == key) {
        b->value = value;
        return;
    }
    cu_insert(t, key, value);
}

bool cu_get (CuckooTable* t, string key, string& value) {
    if (key.empty()) return false;
    Slot* a = &t->t1[hash1(key, t->cap)];
    if (a->key == key) {
        value = a->value;
        return true;
    }
    Slot* b = &t->t2[hash2(key, t->cap)];
    if (b->key == key) {
        value = b->value;
        return true;
    }
    return false;
}

bool cu_remove (CuckooTable* t, string key) {
    if (key.empty()) return false;
    Slot* a = &t->t1[hash1(key, t->cap)];
    if (a->key == key) {
        a->key = "";
        a->value = "";
        t->sz--;
        return true;
    }
    Slot* b = &t->t2[hash2(key, t->cap)];
    if (b->key == key) {
        b->key = "";
        b->value = "";
        t->sz--;
        return true;
    }
    return false;
}

void cu_print (CuckooTable* t) {
    if (t->sz == 0) {
        cout << "Хеш-таблица пуста\n";
        return;
    }
    cout << "Хеш-таблица (кукушка):\n";
    for (int i = 0; i < t->cap; i++) {
        if (!t->t1[i].key.empty()) cout << "T1[" << i << "] " << t->t1[i].key << "=" << t->t1[i].value << endl;
    }
    for (int i = 0; i < t->cap; i++) {
        if (!t->t2[i].key.empty()) cout << "T2[" << i << "] " << t->t2[i].key << "=" << t->t2[i].value << endl;
    }
}

bool genealogy (const vector<string>& t) {
    int n = 0;
    try {
        if (t.size() >= 2) n = stoi(t[1]);
    } catch (...) {}
    if (n < 2) {
        cout << "Ошибка: N должно быть целым числом не меньше 2\n";
        return false;
    }
    if (t.size() != 2 + 2 * (size_t)(n - 1)) {
        cout << "Ошибка: Неверное число пар «потомок родитель» (для N элементов нужно N-1 пар)\n";
        return false;
    }

    ChainTable parent(2 * n + 1);
    ChainTable kids(2 * n + 1);
    ChainTable seen(2 * n + 1);
    vector<string> names;

    for (int i = 0; i < n - 1; i++) {
        string child = t[2 + 2 * i];
        string father = t[3 + 2 * i];
        string tmp;
        if (ct_get(&parent, child, tmp)) {
            cout << "Ошибка: У элемента " << child << " больше одного родителя\n";
            return false;
        }
        ct_put(&parent, child, father);

        string list;
        ct_get(&kids, father, list);
        ct_put(&kids, father, list + child + " ");

        if (!ct_get(&seen, child, tmp)) {
            ct_put(&seen, child, "1");
            names.push_back(child);
        }
        if (!ct_get(&seen, father, tmp)) {
            ct_put(&seen, father, "1");
            names.push_back(father);
        }
    }

    string root;
    int roots = 0;
    for (size_t i = 0; i < names.size(); i++) {
        string tmp;
        if (!ct_get(&parent, names[i], tmp)) {
            root = names[i];
            roots++;
        }
    }
    if (roots == 0) {
        cout << "Ошибка: Родоначальник не найден (в данных есть цикл)\n";
        return false;
    }
    if (roots > 1) {
        cout << "Ошибка: В данных больше одного родоначальника\n";
        return false;
    }

    vector<string> order;
    order.push_back(root);
    for (size_t i = 0; i < order.size(); i++) {
        string list;
        if (!ct_get(&kids, order[i], list)) continue;
        istringstream ss(list);
        string child;
        while (ss >> child) order.push_back(child);
    }
    if (order.size() != names.size()) {
        cout << "Ошибка: В данных есть цикл, не все элементы связаны с родоначальником\n";
        return false;
    }

    ChainTable counts(2 * n + 1);
    for (size_t i = 0; i < order.size(); i++) ct_put(&counts, order[i], "0");
    for (int i = (int)order.size() - 1; i > 0; i--) {
        string p, mine, theirs;
        ct_get(&parent, order[i], p);
        ct_get(&counts, order[i], mine);
        ct_get(&counts, p, theirs);
        ct_put(&counts, p, to_string(stoi(theirs) + stoi(mine) + 1));
    }

    vector<string> sorted = names;
    sort(sorted.begin(), sorted.end());
    for (size_t i = 0; i < sorted.size(); i++) {
        string cnt;
        ct_get(&counts, sorted[i], cnt);
        cout << sorted[i] << " " << cnt << endl;
    }

    cout << "\nДрево:\n";
    vector<string> stack_names;
    vector<int> stack_depth;
    stack_names.push_back(root);
    stack_depth.push_back(0);
    while (!stack_names.empty()) {
        string name = stack_names.back();
        int depth = stack_depth.back();
        stack_names.pop_back();
        stack_depth.pop_back();
        cout << string(depth * 2, ' ') << name << endl;

        string list;
        if (!ct_get(&kids, name, list)) continue;
        vector<string> children;
        istringstream ss(list);
        string child;
        while (ss >> child) children.push_back(child);
        sort(children.begin(), children.end());
        for (int k = (int)children.size() - 1; k >= 0; k--) {
            stack_names.push_back(children[k]);
            stack_depth.push_back(depth + 1);
        }
    }
    return true;
}
