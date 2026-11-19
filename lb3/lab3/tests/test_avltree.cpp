#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "avltree.h"
#include "capture.h"

using namespace std;

using Ints = vector<int>;

namespace {

struct Plain {
    int key;
    Plain* l;
    Plain* r;
    explicit Plain(int k) : key(k), l(nullptr), r(nullptr) {}
};

Plain* plain_insert(Plain* t, int k) {
    if (t == nullptr) return new Plain(k);
    if (k < t->key) t->l = plain_insert(t->l, k);
    else t->r = plain_insert(t->r, k);
    return t;
}

int plain_height(const Plain* t) {
    return t == nullptr ? 0 : 1 + max(plain_height(t->l), plain_height(t->r));
}

bool plain_balanced(const Plain* t) {
    if (t == nullptr) return true;
    int d = plain_height(t->l) - plain_height(t->r);
    return d >= -1 && d <= 1 && plain_balanced(t->l) && plain_balanced(t->r);
}

Ints plain_level_order(const Plain* t) {
    Ints res;
    queue<const Plain*> q;
    if (t != nullptr) q.push(t);
    while (!q.empty()) {
        const Plain* n = q.front();
        q.pop();
        res.push_back(n->key);
        if (n->l != nullptr) q.push(n->l);
        if (n->r != nullptr) q.push(n->r);
    }
    return res;
}

void plain_free(Plain* t) {
    if (t == nullptr) return;
    plain_free(t->l);
    plain_free(t->r);
    delete t;
}

struct Shape {
    bool consistent;   
    bool balanced;     
    int height;
};

Shape analyse(const Ints& level_order) {
    Plain* root = nullptr;
    for (int k : level_order) root = plain_insert(root, k);
    Shape s;
    s.consistent = (plain_level_order(root) == level_order);
    s.balanced = plain_balanced(root);
    s.height = plain_height(root);
    plain_free(root);
    return s;
}

void check_valid(AVLTree& t) {
    Shape s = analyse(t.t_items());
    BOOST_CHECK(s.consistent);
    BOOST_CHECK(s.balanced);
}

void check_items(AVLTree& t, const Ints& expected) {
    Ints actual = t.t_items();
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

int printed_height(AVLTree& t) {
    string out = capture_output([&] { t.t_print_wide(); });
    return static_cast<int>(count(out.begin(), out.end(), '\n'));
}

}  

BOOST_AUTO_TEST_SUITE(AVLTreeTests)

BOOST_AUTO_TEST_CASE(empty_tree) {
    AVLTree t;
    BOOST_CHECK(!t.t_search(1));
    BOOST_CHECK(t.t_items().empty());
    string out = capture_output([&] { t.t_print_wide(); });
    BOOST_CHECK_EQUAL(out, "Дерево пусто\n");
}

BOOST_AUTO_TEST_CASE(insert_and_search_single_key) {
    AVLTree t;
    t.t_insert(42);
    BOOST_CHECK(t.t_search(42));
    BOOST_CHECK(!t.t_search(41));
    check_items(t, Ints{42});
}

BOOST_AUTO_TEST_CASE(duplicates_are_ignored) {
    AVLTree t;
    t.t_insert(5);
    t.t_insert(5);
    t.t_insert(5);
    check_items(t, Ints{5});
}

BOOST_AUTO_TEST_CASE(left_left_case_rotates_right) {
    AVLTree t;
    for (int k : {3, 2, 1}) t.t_insert(k);
    check_items(t, Ints{2, 1, 3});
}

BOOST_AUTO_TEST_CASE(right_right_case_rotates_left) {
    AVLTree t;
    for (int k : {1, 2, 3}) t.t_insert(k);
    check_items(t, Ints{2, 1, 3});
}

BOOST_AUTO_TEST_CASE(left_right_case_double_rotation) {
    AVLTree t;
    for (int k : {3, 1, 2}) t.t_insert(k);
    check_items(t, Ints{2, 1, 3});
}

BOOST_AUTO_TEST_CASE(right_left_case_double_rotation) {
    AVLTree t;
    for (int k : {1, 3, 2}) t.t_insert(k);
    check_items(t, Ints{2, 1, 3});
}

BOOST_AUTO_TEST_CASE(large_ascending_sequence_stays_balanced) {
    AVLTree t;
    for (int k = 1; k <= 1000; ++k) t.t_insert(k);
    Shape s = analyse(t.t_items());
    BOOST_CHECK(s.consistent);
    BOOST_CHECK(s.balanced);
    BOOST_CHECK(s.height >= 10);   
    BOOST_CHECK(s.height <= 14);   
    for (int k = 1; k <= 1000; ++k) BOOST_REQUIRE(t.t_search(k));
    BOOST_CHECK(!t.t_search(0));
    BOOST_CHECK(!t.t_search(1001));
}

BOOST_AUTO_TEST_CASE(delete_missing_key_prints_error) {
    AVLTree t;
    t.t_insert(1);
    string out = capture_output([&] { t.t_delete(99); });
    BOOST_CHECK_EQUAL(out, "Ошибка: Не найдено\n");
    check_items(t, Ints{1});
}

BOOST_AUTO_TEST_CASE(delete_leaf_node_with_one_child_and_node_with_two_children) {
    AVLTree t;
    for (int k : {10, 5, 15, 3, 7, 12, 20}) t.t_insert(k);

    t.t_delete(3);     
    BOOST_CHECK(!t.t_search(3));
    check_valid(t);

    t.t_delete(5);     
    BOOST_CHECK(!t.t_search(5));
    BOOST_CHECK(t.t_search(7));
    check_valid(t);

    t.t_delete(15);    
    BOOST_CHECK(!t.t_search(15));
    BOOST_CHECK(t.t_search(12));
    BOOST_CHECK(t.t_search(20));
    check_valid(t);
}

BOOST_AUTO_TEST_CASE(delete_root_and_last_element) {
    AVLTree t;
    for (int k : {2, 1, 3}) t.t_insert(k);
    t.t_delete(2);
    BOOST_CHECK(!t.t_search(2));
    check_valid(t);
    t.t_delete(1);
    t.t_delete(3);
    BOOST_CHECK(t.t_items().empty());

    t.t_insert(7);
    check_items(t, Ints{7});
}

BOOST_AUTO_TEST_CASE(delete_triggers_rotation) {
    AVLTree t;
    for (int k : {2, 1, 4, 3, 5}) t.t_insert(k);
    t.t_delete(1);   
    check_items(t, Ints{4, 2, 5, 3});
    check_valid(t);
}

BOOST_AUTO_TEST_CASE(print_wide_prints_level_by_level) {
    AVLTree t;
    for (int k : {5, 3, 8, 1, 4, 9, 10}) t.t_insert(k);
    string out = capture_output([&] { t.t_print_wide(); });
    BOOST_CHECK_EQUAL(out,
                      "Уровень 1: 5 \n"
                      "Уровень 2: 3 9 \n"
                      "Уровень 3: 1 4 8 10 \n");
}

BOOST_AUTO_TEST_CASE(random_inserts_and_deletes_match_std_set) {
    mt19937 gen(12345);
    uniform_int_distribution<int> dist(0, 4999);
    AVLTree t;
    set<int> ref;

    for (int i = 0; i < 2000; ++i) {
        int k = dist(gen);
        t.t_insert(k);
        ref.insert(k);
        if (i % 250 == 0) check_valid(t);
    }
    check_valid(t);

    for (int i = 0; i < 1500; ++i) {
        int k = dist(gen);
        if (ref.count(k) != 0) {
            t.t_delete(k);
            ref.erase(k);
        }
        if (i % 250 == 0) check_valid(t);
    }
    check_valid(t);

    Ints actual = t.t_items();
    sort(actual.begin(), actual.end());
    Ints expected(ref.begin(), ref.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());

    for (int k = 0; k < 5000; ++k) {
        if (t.t_search(k) != (ref.count(k) != 0)) {
            BOOST_ERROR("search mismatch for key " << k);
            break;
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
