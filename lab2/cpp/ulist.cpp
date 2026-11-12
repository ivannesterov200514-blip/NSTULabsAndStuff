#include "ulist.h"

static Node* ul_find(UList* l, string key) {
    Node* cur = l->head;
    while (cur != nullptr && cur->data != key) {
        cur = cur->next;
    }
    return cur;
}

void ul_insert_head (UList* l, string val) {
    Node* n = new Node{val, l->head};
    l->head = n;
    l->sz++;
}

bool ul_contains (UList* l, string key) {
    return ul_find(l, key) != nullptr;
}

bool ul_remove_key (UList* l, string key) {
    if (l->head == nullptr) return false;
    if (l->head->data == key) {
        Node* v = l->head;
        l->head = v->next;
        delete v;
        l->sz--;
        return true;
    }
    Node* cur = l->head;
    while (cur->next != nullptr && cur->next->data != key) {
        cur = cur->next;
    }
    if (cur->next == nullptr) return false;
    Node* v = cur->next;
    cur->next = v->next;
    delete v;
    l->sz--;
    return true;
}
