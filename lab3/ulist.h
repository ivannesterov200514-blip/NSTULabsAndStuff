#pragma once
#include <iostream>
#include <string>

using namespace std;

class UList {
private:
    class Node {
    private:
        string data;
        Node* next;

        friend class UList;
    public:
        Node(string val, Node* t) : data(val), next(t) {}
    };

    Node* head;
    int sz;
public:
    UList(Node* n = nullptr);

    Node* ul_find(string key);

    void ul_insert_head (string val);
    void ul_insert_tail (string val);
    void ul_insert_before (string key, string val);
    void ul_insert_after (string key, string val);

    void ul_delete_head ();
    void ul_delete_tail ();
    void ul_delete_before (string key);
    void ul_delete_after (string key);
    void ul_delete_key (string key);

    string ul_search(string key);
    void ul_print_iter();
    void printrecurs(Node* c);
    void ul_print_rec();
    
    ~UList();
};