#include "queue.h"

Queue::QNode::QNode(string val, QNode* t) : data(val), next(t) {}

Queue::Queue(QNode* n) : head(n), tail(n), sz(n ? 1 : 0) {}

void Queue::q_push(string val) {
    QNode* n = new QNode{val, nullptr};
    if (tail == nullptr) {
        head = n;
        tail = n;
    } else {
        tail->next = n;
        tail = n;
    }
    sz++;
}

void Queue::q_pop() {
    if (head == nullptr) {
        cout << "Ошибка: Очередь пуста\n";
        return;
    }
    QNode* v = head;
    head = v->next;
    if (head == nullptr) {
        tail = nullptr;
    }
    cout << v->data << endl;
    delete v;
    sz--;
}

void Queue::q_print() {
    if (head == nullptr) {
        cout << "Очередь пуста\n";
        return;
    }
    cout << "Head: ";
    QNode* cur = head;
    while (cur != nullptr) {
        cout << cur->data << " -> ";
        cur = cur->next;
    }
    cout << ": Tail\n";
}

vector<string> Queue::q_items() const {
    vector<string> res;
    for (QNode* cur = head; cur != nullptr; cur = cur->next) res.push_back(cur->data);
    return res;
}

Queue::~Queue() {
    while (head != nullptr) {
        QNode* v = head;
        head = head->next;
        delete v;
    }
}