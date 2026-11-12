#include "zmeika.h"
#include <algorithm>
#include <vector>

static int getheight(TNode* t) {
    return t ? t->height : 0;
}

static int getbalance(TNode* t) {
    return t ? getheight(t->left) - getheight(t->right) : 0;
}

static TNode* rotate_right (TNode* y) {
    TNode* x = y->left;
    TNode* t = x->right;
    x->right = y;
    y->left = t;
    y->height = max(getheight(y->left), getheight(y->right)) + 1;
    x->height = max(getheight(x->left), getheight(x->right)) + 1;
    return x;
}

static TNode* rotate_left (TNode* x) {
    TNode* y = x->right;
    TNode* t = y->left;
    y->left = x;
    x->right = t;
    x->height = max(getheight(x->left), getheight(x->right)) + 1;
    y->height = max(getheight(y->left), getheight(y->right)) + 1;
    return y;
}

static TNode* t_insert_node(TNode* t, int val) {
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

void t_insert(AVLTree* t, int val) {
    t->root = t_insert_node(t->root, val);
}

static void print_level(TNode* t, int level) {
    if (t == nullptr) return;
    if (level == 1) {
        cout << t->key << " ";
        return;
    }
    print_level(t->left, level - 1);
    print_level(t->right, level - 1);
}

void t_print_wide(AVLTree* t) {
    if (t->root == nullptr) {
        cout << "Дерево пусто\n";
        return;
    }
    int h = getheight(t->root);
    for (int i = 1; i <= h; ++i) {
        cout << "Уровень " << i << ": ";
        print_level(t->root, i);
        cout << endl;
    }
}

static void t_free(TNode* t) {
    if (t == nullptr) return;
    t_free(t->left);
    t_free(t->right);
    delete t;
}

AVLTree::~AVLTree() {
    t_free(root);
}

void tq_push (TQueue* q, TNode* node) {
    TQNode* n = new TQNode(node);
    if (q->tail == nullptr) {
        q->head = n;
        q->tail = n;
    } else {
        q->tail->next = n;
        q->tail = n;
    }
    q->sz++;
}

bool tq_pop (TQueue* q, TNode*& node) {
    if (q->head == nullptr) return false;
    TQNode* v = q->head;
    node = v->node;
    q->head = v->next;
    if (q->head == nullptr) {
        q->tail = nullptr;
    }
    delete v;
    q->sz--;
    return true;
}

void t_snake (AVLTree* t) {
    if (t->root == nullptr) {
        cout << "Дерево пусто\n";
        return;
    }
    TQueue q;
    tq_push(&q, t->root);
    bool backwards = false;
    bool first = true;
    while (q.head != nullptr) {
        int count = q.sz;
        vector<int> level;
        for (int i = 0; i < count; i++) {
            TNode* cur = nullptr;
            tq_pop(&q, cur);
            level.push_back(cur->key);
            if (cur->left != nullptr) tq_push(&q, cur->left);
            if (cur->right != nullptr) tq_push(&q, cur->right);
        }
        for (int i = 0; i < count; i++) {
            int k = backwards ? count - 1 - i : i;
            if (!first) cout << " – ";
            cout << level[k];
            first = false;
        }
        backwards = !backwards;
    }
    cout << endl;
}