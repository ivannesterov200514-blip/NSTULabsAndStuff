#pragma once
#include <iostream>
#include <string>

using namespace std;

struct TNode {
    string key;
    TNode* left;
    TNode* right;
    int height;
    TNode(string k) : key(k), left(nullptr), right(nullptr), height(1) {}
};

struct AVLTree {
    TNode* root;
    AVLTree(TNode* n = nullptr) : root(n) {}
    ~AVLTree(); 
};

void t_insert(AVLTree* t, string val);
void t_delete(AVLTree* t, string val);
bool t_search(AVLTree* t, string val);
void t_print_wide(AVLTree* t);