#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class DList {
private:
    class SNode {
    private:
        string data;
        SNode* next;
        SNode* prev;

        friend class DList;
    public:
        SNode(string val, SNode* nx, SNode* pv);
    };
    SNode* head;
    SNode* tail;
    int sz;
public:
    DList(SNode* n = nullptr);
    // Владеем памятью: запрещаем поверхностное копирование и double free.
    DList(const DList&) = delete;
    DList& operator=(const DList&) = delete;
    SNode* dl_find(string key);
    void dl_unlink(SNode* v);

    void dl_insert_head (string val);
    void dl_insert_tail (string val);
    void dl_insert_before (string key, string val);
    void dl_insert_after (string key, string val);

    void dl_delete_head ();
    void dl_delete_tail ();
    void dl_delete_before (string key);
    void dl_delete_after (string key);
    void dl_delete_key (string key);

    string dl_search(string key);
    void dl_print_iter();
    void printrecurs(SNode* c);
    void dl_print_rec();
    vector<string> dl_items() const;
    ~DList();
};
