#pragma once
#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

struct ANode {
    uint64_t mass;
    char dir;
    ANode* next;
    ANode(uint64_t kg, char d, ANode* t) : mass(kg), dir(d), next(t) {}
};

struct AStack {
    ANode* top;
    int sz;
    AStack(ANode* n = nullptr) : top(n), sz(n ? 1 : 0) {}
    ~AStack() {
        while (top != nullptr) {
            ANode* v = top;
            top = top->next;
            delete v;
        }
    }
};

void ast_push (AStack* s, uint64_t mass, char dir);
bool ast_pop (AStack* s, uint64_t& mass, char& dir);
bool ast_peek (AStack* s, uint64_t& mass, char& dir);

bool asteroids_parse (AStack* in, string text);
void asteroids (AStack* in, AStack* out);
void asteroids_print (AStack* out);
