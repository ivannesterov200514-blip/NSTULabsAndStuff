#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class AVLTree {
private:
    class TNode {
    private:
        int key;
        TNode* left;
        TNode* right;
        int height;

        friend class AVLTree;
    public:
        TNode(int k);
    };
    TNode* root;
public:
    AVLTree(TNode* n = nullptr);
    // Владеем памятью: запрещаем поверхностное копирование и double free.
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;
    int getheight(TNode* t);
    int getbalance(TNode* t);
    TNode* rotate_right (TNode* y);
    TNode* rotate_left (TNode* x);
    TNode* t_insert_node(TNode* t, int val);
    TNode* min_node(TNode* t);
    TNode* t_delete_node(TNode* t, int val);
    bool t_search_node(TNode* t, int val);

    void print_level(TNode* t, int level);

    void t_insert(int val);
    void t_delete(int val);
    bool t_search(int val);
    void t_print_wide();
    vector<int> t_items() const;

    void t_free(TNode* t);
    ~AVLTree();
};
