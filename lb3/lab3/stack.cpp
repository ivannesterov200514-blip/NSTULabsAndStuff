#include "stack.h"

Stack::STNode::STNode(string val, STNode* t) : data(val), next(t) {}

Stack::Stack(STNode* n) : top(n), sz(n ? 1 : 0) {}

void Stack::st_push(string val) {
    STNode* n = new STNode{val, top};
    top = n;
    sz++;
}

void Stack::st_pop() {
    if (top == nullptr) {
        cout << "Ошибка: Стек пуст\n";
        return;
    }
    STNode* v = top;
    top = v->next;
    cout << v->data << endl;
    delete v;
    sz--;
}

void Stack::st_print() {
    if (top == nullptr) {
        cout << "Стек пуст\n";
        return;
    }
    STNode* curr = top;
    while (curr != nullptr) {
        cout << curr->data << endl;
        curr = curr->next;
    }
    cout << "nullptr\n";
}

vector<string> Stack::st_items() const {
    vector<string> res;
    for (STNode* cur = top; cur != nullptr; cur = cur->next) res.push_back(cur->data);
    return res;
}

Stack::~Stack() {
    while (top != nullptr) {
        STNode* v = top;
        top = top->next;
        delete v;
    }
}