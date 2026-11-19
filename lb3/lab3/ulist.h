#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class UList {
private:
    class Node {
    private:
        string data;
        string value;
        Node* next;

        friend class UList;
    public:
        Node(string val, Node* t);
        Node(string key, string val, Node* t);
    };

    Node* head;
    int sz;

public:
    UList(Node* n = nullptr);
    UList(const UList& other) = delete;
    UList& operator=(const UList& other) = delete;

    Node* ul_find(string key);

    void ul_insert_head(string val);
    void ul_insert_head(string key, string value);
    void ul_insert_tail(string val);
    void ul_insert_before(string key, string val);
    void ul_insert_after(string key, string val);

    void ul_delete_head();
    void ul_delete_tail();
    void ul_delete_before(string key);
    void ul_delete_after(string key);
    void ul_delete_key(string key);

    string ul_search(string key);
    void ul_print_iter();
    void printrecurs(Node* c);
    void ul_print_rec();

    bool ul_get_value(string key, string& value) const;
    bool ul_update_value(string key, string value);
    bool ul_remove_key(string key);
    vector<pair<string, string>> ul_get_pairs() const;
    void ul_print_pairs() const;
    int ul_getsz() const;

    ~UList();
};