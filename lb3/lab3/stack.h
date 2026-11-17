#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Stack {
private:
    class STNode {
    private:
        string data;
        STNode* next;

        friend class Stack;
    public:
        STNode(string val, STNode* t);
    };
    STNode* top;
    int sz;
public:
    Stack(STNode* n = nullptr);
    // Владеем памятью: запрещаем поверхностное копирование и double free.
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    void st_push(string val);
    void st_pop();
    void st_print();
    vector<string> st_items() const;
    ~Stack();
};
