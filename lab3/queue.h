#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Queue {
private:
    class QNode {
    private:
        string data;
        QNode* next;

        friend class Queue;
    public:
        QNode(string val, QNode* t);
    };
    QNode* head;
    QNode* tail;
    int sz;
public:
    Queue(QNode* n = nullptr);
    void q_push(string val);
    void q_pop();
    void q_print();
    vector<string> q_items() const;
    ~Queue();
};