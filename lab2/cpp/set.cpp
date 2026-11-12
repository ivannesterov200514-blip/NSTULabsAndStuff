#include "set.h"

static int set_hash (Set* s, string val) {
    uint64_t h = 5381;
    for (size_t i = 0; i < val.size(); i++) {
        h = h * 33 + (unsigned char)val[i];
    }
    return (int)(h % s->cap);
}

void set_add (Set* s, string val) {
    UList* b = &s->table[set_hash(s, val)];
    if (ul_contains(b, val)) {
        cout << "Ошибка: Элемент уже есть в множестве\n";
        return;
    }
    ul_insert_head(b, val);
    s->sz++;
}

void set_del (Set* s, string val) {
    UList* b = &s->table[set_hash(s, val)];
    if (!ul_remove_key(b, val)) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    s->sz--;
}

bool set_at (Set* s, string val) {
    return ul_contains(&s->table[set_hash(s, val)], val);
}

void set_print (Set* s) {
    if (s->sz == 0) {
        cout << "Множество пусто\n";
        return;
    }
    cout << "Множество: ";
    for (int i = 0; i < s->cap; i++) {
        Node* cur = s->table[i].head;
        while (cur != nullptr) {
            cout << cur->data << " ";
            cur = cur->next;
        }
    }
    cout << endl;
}

static bool to_num (const string& s, uint64_t& x) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); i++) {
        if (!isdigit((unsigned char)s[i])) return false;
    }
    try {
        x = stoull(s);
    } catch (...) {
        return false;
    }
    return true;
}

bool turtles (const vector<string>& t, int& answer) {
    uint64_t n;
    if (t.size() < 2 || !to_num(t[1], n)) {
        cout << "Ошибка: N должно быть неотрицательным целым числом\n";
        return false;
    }
    size_t count = t.size() - 2;
    if (count % 2 != 0 || count / 2 != n) {
        cout << "Ошибка: Для " << n << " черепах нужно " << n << " пар чисел (a и b для каждой)\n";
        return false;
    }

    Set places;
    for (uint64_t i = 0; i < n; i++) {
        uint64_t a, b;
        if (!to_num(t[2 + 2 * i], a) || !to_num(t[3 + 2 * i], b)) {
            cout << "Ошибка: Числа a и b должны быть целыми неотрицательными\n";
            return false;
        }
        if (a <= n - 1 && b == n - 1 - a) {
            string place = to_string(a);
            if (!set_at(&places, place)) set_add(&places, place);
        }
    }
    answer = places.sz;
    return true;
}
