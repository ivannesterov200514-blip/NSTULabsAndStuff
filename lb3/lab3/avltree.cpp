#include "avltree.h"
#include <algorithm>

AVLTree::TNode::TNode(int k) : key(k), left(nullptr), right(nullptr), height(1) {}

AVLTree::AVLTree(TNode* n) : root(n) {}

int AVLTree::getheight(TNode* t) {
    return t ? t->height : 0;
}

int AVLTree::getbalance(TNode* t) {
    return t ? getheight(t->left) - getheight(t->right) : 0;
}

AVLTree::TNode* AVLTree::rotate_right (TNode* y) {
    TNode* x = y->left;
    TNode* t = x->right;
    x->right = y;
    y->left = t;
    y->height = max(getheight(y->left), getheight(y->right)) + 1;
    x->height = max(getheight(x->left), getheight(x->right)) + 1;
    return x;
}

AVLTree::TNode* AVLTree::rotate_left (TNode* x) {
    TNode* y = x->right;
    TNode* t = y->left;
    y->left = x;
    x->right = t;
    x->height = max(getheight(x->left), getheight(x->right)) + 1;
    y->height = max(getheight(y->left), getheight(y->right)) + 1;
    return y;
}

AVLTree::TNode* AVLTree::t_insert_node(TNode* t, int val) {
    if (t == nullptr) 
        return new TNode(val);
    if (val < t->key)
        t->left = t_insert_node(t->left, val);
    else if (val > t->key)
        t->right = t_insert_node(t->right, val);
    else
        return t; 

    t->height = 1 + max(getheight(t->left), getheight(t->right));
    int balance = getbalance(t);

    if (balance > 1 && val < t->left->key)
        return rotate_right(t);

    if (balance < -1 && val > t->right->key)
        return rotate_left(t);

    if (balance > 1 && val > t->left->key) {
        t->left = rotate_left(t->left);
        return rotate_right(t);
    }

    if (balance < -1 && val < t->right->key) {
        t->right = rotate_right(t->right);
        return rotate_left(t);
    }
    return t;
}

AVLTree::TNode* AVLTree::min_node(TNode* t){
    while (t->left != nullptr) {
        t = t->left;
    }
    return t;
}

AVLTree::TNode* AVLTree::t_delete_node(TNode* t, int val) {
    if (t == nullptr) return t;

    if (val < t->key) {
        t->left = t_delete_node(t->left, val);
    } else if (val > t->key) {
        t->right = t_delete_node(t->right, val);
    } else {
        if (t->left == nullptr || t->right == nullptr) {
            TNode* c = t->left ? t->left : t->right;
            delete t;
            return c;
        }
        TNode* m = min_node(t->right);
        t->key = m->key;
        t->right = t_delete_node(t->right, m->key);
    }

    t->height = 1 + max(getheight(t->left), getheight(t->right));
    int balance = getbalance(t);

    if (balance > 1 && getbalance(t->left) >= 0)
        return rotate_right(t);

    if (balance > 1 && getbalance(t->left) < 0) {
        t->left = rotate_left(t->left);
        return rotate_right(t);
    }

    if (balance < -1 && getbalance(t->right) <= 0)
        return rotate_left(t);

    if (balance < -1 && getbalance(t->right) > 0) {
        t->right = rotate_right(t->right);
        return rotate_left(t);
    }
    return t;
}

bool AVLTree::t_search_node(TNode* t, int val) {
    if (t == nullptr) return false;
    if (val == t->key) return true;
    if (val < t->key) return t_search_node(t->left, val);
    return t_search_node(t->right, val);
}

void AVLTree::print_level(TNode* t, int level) {
    if (t == nullptr) return;
    if (level == 1) {
        cout << t->key << " ";
        return;
    }
    print_level(t->left, level - 1);
    print_level(t->right, level - 1);
}

void AVLTree::t_insert(int val) {
    root = t_insert_node(root, val);
}

void AVLTree::t_delete(int val) {
    if (!t_search(val)) {
        cout << "Ошибка: Не найдено\n";
        return;
    }
    root = t_delete_node(root, val);
}

bool AVLTree::t_search(int val) {
    return t_search_node(root, val);
}

void AVLTree::t_print_wide() {
    if (root == nullptr) {
        cout << "Дерево пусто\n";
        return;
    }
    int h = getheight(root);
    for (int i = 1; i <= h; ++i) {
        cout << "Уровень " << i << ": ";
        print_level(root, i);
        cout << endl;
    }
}


vector<int> AVLTree::t_items() const {
    vector<int> res;
    vector<TNode*> q;
    if (root != nullptr) q.push_back(root);
    for (size_t i = 0; i < q.size(); ++i) {
        res.push_back(q[i]->key);
        if (q[i]->left != nullptr) q.push_back(q[i]->left);
        if (q[i]->right != nullptr) q.push_back(q[i]->right);
    }
    return res;
}

void AVLTree::t_free(TNode* t){
    if (t == nullptr) return;
    t_free(t->left);
    t_free(t->right);
    delete t;
}

AVLTree::~AVLTree() {
    t_free(root);
}