#pragma once
#include <iostream>
#include <string>

using namespace std;

struct Node {
    string data;
    Node* next;
    Node(string val, Node* t) : data(val), next(t) {}
};

struct UList {
    Node* head;
    int sz;
    UList(Node* n = nullptr) : head(n), sz(n ? 1 : 0) {}
    ~UList() {
        while (head != nullptr) {
            Node* v = head;
            head = head->next;
            delete v;
        }
    }
};

void ul_insert_head (UList* l, string val);
bool ul_contains (UList* l, string key);
bool ul_remove_key (UList* l, string key);
