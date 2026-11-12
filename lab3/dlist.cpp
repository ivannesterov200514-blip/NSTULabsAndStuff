#include "dlist.h"

DList::SNode::SNode(string val, SNode* nx, SNode* pv) : data(val), next(nx), prev(pv) {}

DList::DList(SNode* n = nullptr) : head(n), tail(n), sz(n ? 1 : 0) {}

DList::SNode* DList::dl_find(string key) {
    SNode* cur = head;
    while (cur != nullptr && cur->data != key) {
        cur = cur->next;
    }
    return cur;
}

void DList::dl_unlink(SNode* v) {
    if (v->prev != nullptr) v->prev->next = v->next;
    else head = v->next;
    if (v->next != nullptr) v->next->prev = v->prev;
    else tail = v->prev;
    if (head == nullptr) tail = nullptr;
    delete v;
    sz--;
}

void DList::dl_insert_head (string val) {
    SNode* n = new SNode(val, head, nullptr);
    if (head != nullptr) head->prev = n;
    else tail = n;
    head = n;
    sz++;
}

void DList::dl_insert_tail (string val) {
    SNode* n = new SNode(val, nullptr, tail);
    if (tail != nullptr) tail->next = n;
    else head = n;
    tail = n;
    sz++;
}

void DList::dl_insert_before (string key, string val) {
    SNode* cur = dl_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    SNode* n = new SNode(val, cur, cur->prev);
    if (cur->prev != nullptr) cur->prev->next = n;
    else head = n;
    cur->prev = n;
    sz++;
}

void DList::dl_insert_after (string key, string val) {
    SNode* cur = dl_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    SNode* n = new SNode(val, cur->next, cur);
    if (cur->next != nullptr) cur->next->prev = n;
    cur->next = n;
    if (n->next == nullptr) tail = n;
    sz++;
}

void DList::dl_delete_head () {
    if (head == nullptr) {
        cout << "Ошибка: Список пуст\n";
        return;
    }
    if (head->next == nullptr) {
        cout << "Последний узел списка удален.\n";
    }
    dl_unlink(head);
}

void DList::dl_delete_tail () {
    if (tail == nullptr) {
        cout << "Ошибка: Список пуст\n";
        return;
    }
    if (head == tail) {
        cout << "Последний узел списка удален.\n";
    }
    dl_unlink(tail);
}

void DList::dl_delete_before (string key) {
    SNode* cur = dl_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    if (cur->prev == nullptr) {
        cout << "Ошибка: Перед элементом ничего нет\n";
        return;
    }
    dl_unlink(cur->prev);
}

void DList::dl_delete_after (string key) {
    SNode* cur = dl_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    if (cur->next == nullptr) {
        cout << "Ошибка: После элемента ничего нет\n";
        return;
    }
    dl_unlink(cur->next);
}

void DList::dl_delete_key (string key) {
    SNode* cur = dl_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    if (head->next == nullptr) {
        cout << "Последний узел списка удален.\n";
    }
    dl_unlink(cur);
}

string DList::dl_search(string key) {
    SNode* cur = dl_find(key);
    if (cur == nullptr) {
        cout << "Ошибка: Не найдено\n";
        return "";
    }
    cout << cur->data << endl;
    return cur->data;
}

void DList::dl_print_iter() {
    if (head == nullptr) {
        cout << "Список пуст\n";
        return;
    }
    SNode* cur = head;
    cout << "nullptr <-> ";
    while (cur != nullptr) {
        cout << cur->data << " <-> ";
        cur = cur->next;
    }
    cout << "nullptr\n";
}

void DList::printrecurs(SNode* c) {
    if (c == nullptr) {
        cout << "nullptr\n";
        return;
    }
    cout << c->data << " <-> ";
    printrecurs(c->next);
}

void DList::dl_print_rec() {
    if (head == nullptr) {
        cout << "Список пуст\n";
        return;
    }
    cout << "nullptr <-> ";
    printrecurs(head);
}

DList::~DList() {
    while (head != nullptr) {
        SNode* v = head;
        head = head->next;
        delete v;
    }
}