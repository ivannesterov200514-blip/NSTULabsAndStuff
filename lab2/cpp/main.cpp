#include "array.h"
#include "zmeika.h"
#include "hashtable.h"
#include "lfu.h"
#include "asteroids.h"
#include "set.h"

#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstring>
#include <cctype>

struct Db {
    map<string, Set*> sets;
    map<string, AVLTree*> trs;
    map<string, ChainTable*> cts;
    map<string, CuckooTable*> kts;
    map<string, Lfu*> lfus;

    map<string, char> types;

    ~Db() {
        for (auto& p : sets) delete p.second;
        for (auto& p : trs) delete p.second;
        for (auto& p : cts) delete p.second;
        for (auto& p : kts) delete p.second;
        for (auto& p : lfus) delete p.second;
    }
};

template <class T>
static T* find_cont(map<string, T*>& m, const string& name) {
    auto it = m.find(name);
    if (it == m.end()) {
        cout << "Ошибка: Контейнер " << name << " не найден\n";
        return nullptr;
    }
    return it->second;
}

template <class T>
static T* get_or_create(Db& db, map<string, T*>& m, const string& name, char type) {
    auto tp = db.types.find(name);
    if (tp != db.types.end() && tp->second != type) {
        cout << "Ошибка: Имя " << name << " уже занято другой структурой\n";
        return nullptr;
    }
    auto it = m.find(name);
    if (it != m.end()) return it->second;
    T* c = new T();
    m[name] = c;
    db.types[name] = type;
    return c;
}

static bool to_key(const string& s, int& x) {
    bool ok = false;
    try {
        size_t pos = 0;
        x = stoi(s, &pos);
        ok = (pos == s.size());
    } catch (...) {}
    if (!ok) cout << "Ошибка: Ключ дерева должен быть целым числом\n";
    return ok;
}

static bool to_cap(const string& s, int& x) {
    bool ok = false;
    try {
        size_t pos = 0;
        x = stoi(s, &pos);
        ok = (pos == s.size() && x >= 0);
    } catch (...) {}
    if (!ok) cout << "Ошибка: Ёмкость должна быть неотрицательным целым числом\n";
    return ok;
}

static void tree_to_stream(TNode* root, ostream& out) {
    vector<TNode*> q;
    if (root != nullptr) q.push_back(root);
    for (size_t i = 0; i < q.size(); ++i) {
        out << " " << q[i]->key;
        if (q[i]->left != nullptr) q.push_back(q[i]->left);
        if (q[i]->right != nullptr) q.push_back(q[i]->right);
    }
}

static void save_db(Db& db, const string& file) {
    ofstream out(file);
    if (!out) {
        cout << "Ошибка: Не удалось записать файл " << file << "\n";
        return;
    }
    for (auto& p : db.sets) {
        out << "H " << p.first;
        for (int i = 0; i < p.second->cap; ++i) {
            for (Node* cur = p.second->table[i].head; cur != nullptr; cur = cur->next) out << " " << cur->data;
        }
        out << "\n";
    }
    for (auto& p : db.trs) {
        out << "T " << p.first;
        tree_to_stream(p.second->root, out);
        out << "\n";
    }
    for (auto& p : db.cts) {
        out << "C " << p.first;
        for (int i = 0; i < p.second->cap; ++i) {
            for (KVNode* cur = p.second->buckets[i]; cur != nullptr; cur = cur->next) out << " " << cur->key << " " << cur->value;
        }
        out << "\n";
    }
    for (auto& p : db.kts) {
        out << "K " << p.first;
        for (int i = 0; i < p.second->cap; ++i) {
            if (!p.second->t1[i].key.empty()) out << " " << p.second->t1[i].key << " " << p.second->t1[i].value;
            if (!p.second->t2[i].key.empty()) out << " " << p.second->t2[i].key << " " << p.second->t2[i].value;
        }
        out << "\n";
    }
    for (auto& p : db.lfus) {
        out << "U " << p.first << " " << p.second->cap << " " << p.second->clock;
        for (int i = 0; i < p.second->data->cap; ++i) {
            for (KVNode* cur = p.second->data->buckets[i]; cur != nullptr; cur = cur->next) out << " " << cur->key << " " << cur->value;
        }
        out << "\n";
    }
}

static void load_db(Db& db, const string& file) {
    ifstream in(file);
    if (!in) return;

    string line;
    while (getline(in, line)) {
        istringstream ss(line);
        string type, name, tok;
        if (!(ss >> type >> name)) continue;
        vector<string> vals;
        while (ss >> tok) vals.push_back(tok);

        if (type == "H") {
            Set* s = get_or_create(db, db.sets, name, 'H');
            if (s == nullptr) continue;
            for (size_t i = 0; i < vals.size(); ++i) {
                if (!set_at(s, vals[i])) set_add(s, vals[i]);
            }
        } else if (type == "T") {
            AVLTree* t = get_or_create(db, db.trs, name, 'T');
            if (t == nullptr) continue;
            for (size_t i = 0; i < vals.size(); ++i) {
                int key;
                if (to_key(vals[i], key)) t_insert(t, key);
            }
        } else if (type == "C" || type == "K") {
            if (vals.size() % 2 != 0) {
                cout << "Ошибка: В строке таблицы " << name << " нечётное число значений\n";
                continue;
            }
            if (type == "C") {
                ChainTable* c = get_or_create(db, db.cts, name, 'C');
                if (c == nullptr) continue;
                for (size_t i = 0; i + 1 < vals.size(); i += 2) ct_put(c, vals[i], vals[i + 1]);
            } else {
                CuckooTable* c = get_or_create(db, db.kts, name, 'K');
                if (c == nullptr) continue;
                for (size_t i = 0; i + 1 < vals.size(); i += 2) cu_put(c, vals[i], vals[i + 1]);
            }
        } else if (type == "U") {
            if (vals.size() < 2 || (vals.size() - 2) % 4 != 0) {
                cout << "Ошибка: Неверная строка кэша " << name << " в файле\n";
                continue;
            }
            Lfu* c = get_or_create(db, db.lfus, name, 'U');
            if (c == nullptr) continue;
            try {
                c->cap = stoi(vals[0]);
                c->clock = stoull(vals[1]);
                for (size_t i = 2; i + 3 < vals.size(); i += 4) lfu_put(c, vals[i], vals[i + 1], stoull(vals[i + 2]), stoull(vals[i + 3]));
            } catch (...) {
                cout << "Ошибка: Неверная строка кэша " << name << " в файле\n";
            }
        } else {
            cout << "Ошибка: Неизвестный тип структуры в файле: " << type << "\n";
        }
    }
}

static void print_any(Db& db, const string& name) {
    auto tp = db.types.find(name);
    if (tp == db.types.end()) {
        cout << "Ошибка: Контейнер " << name << " не найден\n";
        return;
    }
    switch (tp->second) {
        case 'H': set_print(db.sets[name]); break;
        case 'T': t_print_wide(db.trs[name]); break;
        case 'C': ct_print(db.cts[name]); break;
        case 'K': cu_print(db.kts[name]); break;
        case 'U': lfu_print(db.lfus[name]); break;
    }
}

static char hash_kind(Db& db, const string& name) {
    auto tp = db.types.find(name);
    if (tp == db.types.end()) {
        cout << "Ошибка: Контейнер " << name << " не найден\n";
        return 0;
    }
    if (tp->second != 'C' && tp->second != 'K') {
        cout << "Ошибка: " << name << " не является хеш-таблицей\n";
        return 0;
    }
    return tp->second;
}

enum class Cmd {
    Print,
    SetCreate, SetAdd, SetDel, SetAt,
    TCreate, TInsert, TSnake,
    HCreate, HSet, HGet, HDel, HStats,
    Genealogy,
    LfuCreate, LfuSet, LfuGet,
    Asteroids,
    Turtles,
    Gistogram
};

struct CmdInfo {
    Cmd cmd;
    size_t argc;
    bool changes;
};

static const CmdInfo* find_cmd(const string& name) {
    static const map<string, CmdInfo> table = {
        {"PRINT", {Cmd::Print, 2, false}},

        {"SETCREATE", {Cmd::SetCreate, 2, true}},
        {"SETADD", {Cmd::SetAdd, 3, true}},
        {"SETDEL", {Cmd::SetDel, 3, true}},
        {"SET_AT", {Cmd::SetAt, 3, false}},

        {"TCREATE", {Cmd::TCreate, 2, true}},
        {"TINSERT", {Cmd::TInsert, 3, true}},
        {"TSNAKE", {Cmd::TSnake, 2, false}},

        {"HCREATE", {Cmd::HCreate, 3, true}},
        {"HSET", {Cmd::HSet, 4, true}},
        {"HGET", {Cmd::HGet, 3, false}},
        {"HDEL", {Cmd::HDel, 3, true}},
        {"HSTATS", {Cmd::HStats, 1, false}},

        {"GENEALOGY", {Cmd::Genealogy, 0, false}},

        {"LFUCREATE", {Cmd::LfuCreate, 3, true}},
        {"LFUSET", {Cmd::LfuSet, 4, true}},
        {"LFUGET", {Cmd::LfuGet, 3, true}},

        {"ASTEROIDS", {Cmd::Asteroids, 0, false}},
        {"TURTLES", {Cmd::Turtles, 0, false}},
        {"GISTOGRAM", {Cmd::Gistogram, 0, false}},
    };
    auto it = table.find(name);
    if (it == table.end()) return nullptr;
    return &it->second;
}

static void run(Db& db, Cmd cmd, const vector<string>& t) {
    switch (cmd) {

        case Cmd::Print:
            print_any(db, t[1]);
            break;

        case Cmd::SetCreate:
            get_or_create(db, db.sets, t[1], 'H');
            break;
        case Cmd::SetAdd: {
            Set* s = get_or_create(db, db.sets, t[1], 'H');
            if (s) set_add(s, t[2]);
            break;
        }
        case Cmd::SetDel: {
            Set* s = find_cont(db.sets, t[1]);
            if (s) set_del(s, t[2]);
            break;
        }
        case Cmd::SetAt: {
            Set* s = find_cont(db.sets, t[1]);
            if (s) cout << (set_at(s, t[2]) ? "TRUE" : "FALSE") << endl;
            break;
        }

        case Cmd::TCreate:
            get_or_create(db, db.trs, t[1], 'T');
            break;
        case Cmd::TInsert: {
            AVLTree* tr = get_or_create(db, db.trs, t[1], 'T');
            int key;
            if (tr && to_key(t[2], key)) t_insert(tr, key);
            break;
        }
        case Cmd::TSnake: {
            AVLTree* tr = find_cont(db.trs, t[1]);
            if (tr) t_snake(tr);
            break;
        }

        case Cmd::HCreate: {
            string kind = t[2];
            for (char& c : kind) c = toupper((unsigned char)c);
            if (kind == "CHAIN") get_or_create(db, db.cts, t[1], 'C');
            else if (kind == "CUCKOO") get_or_create(db, db.kts, t[1], 'K');
            else cout << "Ошибка: Тип таблицы должен быть CHAIN или CUCKOO\n";
            break;
        }
        case Cmd::HSet: {
            auto tp = db.types.find(t[1]);
            if (tp == db.types.end()) {
                ChainTable* c = get_or_create(db, db.cts, t[1], 'C');
                if (c) ct_put(c, t[2], t[3]);
            } else {
                char kind = hash_kind(db, t[1]);
                if (kind == 'C') ct_put(db.cts[t[1]], t[2], t[3]);
                if (kind == 'K') cu_put(db.kts[t[1]], t[2], t[3]);
            }
            break;
        }
        case Cmd::HGet: {
            char kind = hash_kind(db, t[1]);
            string value;
            bool ok = false;
            if (kind == 'C') ok = ct_get(db.cts[t[1]], t[2], value);
            if (kind == 'K') ok = cu_get(db.kts[t[1]], t[2], value);
            if (kind != 0) {
                if (ok) cout << value << endl;
                else cout << "Ошибка: Ключ не найден\n";
            }
            break;
        }
        case Cmd::HDel: {
            char kind = hash_kind(db, t[1]);
            bool ok = false;
            if (kind == 'C') ok = ct_remove(db.cts[t[1]], t[2]);
            if (kind == 'K') ok = cu_remove(db.kts[t[1]], t[2]);
            if (kind != 0 && !ok) cout << "Ошибка: Ключ не найден\n";
            break;
        }

        case Cmd::HStats: {
            hash_analysis();
            break;
        }           

        case Cmd::Genealogy:
            genealogy(t);
            break;

        case Cmd::LfuCreate: {
            int cap;
            if (!to_cap(t[2], cap)) break;
            if (db.lfus.count(t[1])) {
                cout << "Ошибка: Кэш " << t[1] << " уже существует\n";
                break;
            }
            Lfu* c = get_or_create(db, db.lfus, t[1], 'U');
            if (c) c->cap = cap;
            break;
        }
        case Cmd::LfuSet: {
            Lfu* c = find_cont(db.lfus, t[1]);
            if (c) lfu_set(c, t[2], t[3]);
            break;
        }
        case Cmd::LfuGet: {
            Lfu* c = find_cont(db.lfus, t[1]);
            if (c) {
                string value;
                if (lfu_get(c, t[2], value)) cout << value << endl;
                else cout << -1 << endl;
            }
            break;
        }

        case Cmd::Asteroids: {
            string text;
            for (size_t i = 1; i < t.size(); ++i) text += t[i] + " ";
            AStack in, out;
            if (asteroids_parse(&in, text)) {
                asteroids(&in, &out);
                asteroids_print(&out);
            }
            break;
        }

        case Cmd::Turtles: {
            int answer;
            if (turtles(t, answer)) cout << answer << endl;
            break;
        }

        case Cmd::Gistogram: {
            Arr arr;
            for (size_t i = 1; i < t.size(); ++i) {
                for (size_t k = 0; k < t[i].size(); ++k) arr_insert_tail(&arr, string(1, t[i][k]));
            }
            gistogram(&arr);
            break;
        }
    }
}

int main(int argc, char* argv[]) {

    string file, query;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) file = argv[++i];
        else if (strcmp(argv[i], "--query") == 0 && i + 1 < argc) query = argv[++i];
    }
    if (file.empty() || query.empty()) {
        cout << "Использование: ./lab2 --file file.data --query 'КОМАНДА имя [аргументы]'\n";
        return 1;
    }

    vector<string> t;
    istringstream ss(query);
    string tok;
    while (ss >> tok) t.push_back(tok);
    if (t.empty()) {
        cout << "Ошибка: Пустой запрос\n";
        return 1;
    }

    for (char& c : t[0]) c = toupper((unsigned char)c);

    const CmdInfo* info = find_cmd(t[0]);
    if (info == nullptr) {
        cout << "Ошибка: Неизвестная команда " << t[0] << "\n";
        return 1;
    }
    if (info->argc != 0 && t.size() != info->argc) {
        cout << "Ошибка: Неверное число аргументов для " << t[0] << "\n";
        return 1;
    }

    Db db;
    load_db(db, file);
    run(db, info->cmd, t);

    if (info->changes) save_db(db, file);

    return 0;
}
