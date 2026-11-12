#pragma once
#include <iostream>
#include <string>

using namespace std;

struct TNode {
    int key;
    TNode* left;
    TNode* right;
    int height;
    TNode(int k) : key(k), left(nullptr), right(nullptr), height(1) {}
};

struct AVLTree {
    TNode* root;
    AVLTree(TNode* n = nullptr) : root(n) {}
    ~AVLTree();
};

void t_insert(AVLTree* t, int val);
void t_print_wide(AVLTree* t);

struct TQNode {
    TNode* node;
    TQNode* next;
    TQNode(TNode* n) : node(n), next(nullptr) {}
};

struct TQueue {
    TQNode* head;
    TQNode* tail;
    int sz;
    TQueue() : head(nullptr), tail(nullptr), sz(0) {}
    ~TQueue() {
        while (head != nullptr) {
            TQNode* v = head;
            head = head->next;
            delete v;
        }
    }
};

void tq_push (TQueue* q, TNode* node);
bool tq_pop (TQueue* q, TNode*& node);
void t_snake (AVLTree* t);