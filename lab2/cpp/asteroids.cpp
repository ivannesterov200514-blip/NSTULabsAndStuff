#include "asteroids.h"
#include <vector>

void ast_push (AStack* s, uint64_t mass, char dir) {
    ANode* n = new ANode(mass, dir, s->top);
    s->top = n;
    s->sz++;
}

bool ast_pop (AStack* s, uint64_t& mass, char& dir) {
    if (s->top == nullptr) return false;
    ANode* v = s->top;
    mass = v->mass;
    dir = v->dir;
    s->top = v->next;
    delete v;
    s->sz--;
    return true;
}

bool ast_peek (AStack* s, uint64_t& mass, char& dir) {
    if (s->top == nullptr) return false;
    mass = s->top->mass;
    dir = s->top->dir;
    return true;
}

bool asteroids_parse (AStack* in, string text) {
    vector<uint64_t> masses;
    vector<char> dirs;
    size_t i = 0;
    while (i < text.size()) {
        char c = text[i];
        if (isdigit((unsigned char)c)) {
            uint64_t v = 0;
            while (i < text.size() && isdigit((unsigned char)text[i])) {
                uint64_t d = text[i] - '0';
                if (v > (UINT64_MAX - d) / 10) {
                    cout << "Ошибка: Размер астероида слишком большой\n";
                    return false;
                }
                v = v * 10 + d;
                i++;
            }
            masses.push_back(v);
            continue;
        }
        if (c == 'R' || c == 'L' || c == 'r' || c == 'l') {
            dirs.push_back((char)toupper((unsigned char)c));
        }
        i++;
    }
    if (masses.size() != dirs.size()) {
        cout << "Ошибка: У каждого астероида должны быть размер и направление (R или L)\n";
        return false;
    }
    for (int k = (int)masses.size() - 1; k >= 0; k--) {
        ast_push(in, masses[k], dirs[k]);
    }
    return true;
}

void asteroids (AStack* in, AStack* out) {
    uint64_t mass;
    char dir;
    while (ast_pop(in, mass, dir)) {
        bool alive = true;
        uint64_t top_mass;
        char top_dir;
        while (dir == 'L' && ast_peek(out, top_mass, top_dir) && top_dir == 'R') {
            ast_pop(out, top_mass, top_dir);
            if (top_mass < mass) {
                mass -= top_mass;
            } else if (top_mass == mass) {
                alive = false;
                break;
            } else {
                ast_push(out, top_mass - mass, 'R');
                alive = false;
                break;
            }
        }
        if (alive) ast_push(out, mass, dir);
    }
}

void asteroids_print (AStack* out) {
    vector<string> items;
    ANode* cur = out->top;
    while (cur != nullptr) {
        items.push_back("('" + to_string(cur->mass) + "', '" + cur->dir + "')");
        cur = cur->next;
    }
    cout << "[";
    for (int k = (int)items.size() - 1; k >= 0; k--) {
        cout << items[k];
        if (k > 0) cout << ", ";
    }
    cout << "]" << endl;
}
