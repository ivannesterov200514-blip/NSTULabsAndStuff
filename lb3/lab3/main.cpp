#include "array.h"
#include "queue.h"
#include "stack.h"
#include "dlist.h"
#include "ulist.h"
#include "avltree.h"
#include "hashtable.h"
#include "binary_storage.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <cstring>
#include <cctype>

struct Db {
    map<string, Arr*> arrs;
    map<string, UList*> uls;
    map<string, DList*> dls;
    map<string, Stack*> sts;
    map<string, Queue*> qs;
    map<string, AVLTree*> trs;
    map<string, ChainTable*> cts;
    map<string, CuckooTable*> cus;

    map<string, char> types;

    ~Db() {
        for (auto& p : arrs) delete p.second;
        for (auto& p : uls) delete p.second;
        for (auto& p : dls) delete p.second;
        for (auto& p : sts) delete p.second;
        for (auto& p : qs) delete p.second;
        for (auto& p : trs) delete p.second;
        for (auto& p : cts) delete p.second;
        for (auto& p : cus) delete p.second;
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

static bool to_int(const string& s, int& x) {
    bool ok = false;
    try {
        size_t pos = 0;
        x = stoi(s, &pos);
        ok = (pos == s.size());
    } catch (...) {}
    if (!ok) cout << "Ошибка: Индекс должен быть числом\n";
    return ok;
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

static vector<BinaryRecord> collect_records(Db& db) {
    vector<BinaryRecord> result;
    for (auto& p : db.arrs) {
        BinaryRecord r{'M', p.first, {}};
        for (int i = 0; i < p.second->arr_getsz(); ++i) r.values.push_back(p.second->arr_search(i));
        result.push_back(r);
    }
    for (auto& p : db.uls) {
        BinaryRecord r{'F', p.first, {}};
        for (auto& kv : p.second->ul_get_pairs()) r.values.push_back(kv.first);
        result.push_back(r);
    }
    for (auto& p : db.dls) result.push_back({'L', p.first, p.second->dl_items()});
    for (auto& p : db.sts) result.push_back({'S', p.first, p.second->st_items()});
    for (auto& p : db.qs) result.push_back({'Q', p.first, p.second->q_items()});
    for (auto& p : db.trs) {
        BinaryRecord r{'T', p.first, {}};
        for (int v : p.second->t_items()) r.values.push_back(to_string(v));
        result.push_back(r);
    }
    for (auto& p : db.cts) {
        BinaryRecord r{'C', p.first, {}};
        for (auto& kv : p.second->ct_get_all()) {
            r.values.push_back(kv.first);
            r.values.push_back(kv.second);
        }
        result.push_back(r);
    }
    for (auto& p : db.cus) {
        BinaryRecord r{'K', p.first, {}};
        for (auto& kv : p.second->cu_get_all()) {
            r.values.push_back(kv.first);
            r.values.push_back(kv.second);
        }
        result.push_back(r);
    }
    return result;
}

static bool is_binary_file(const string& path) {
    return path.size() >= 4 && path.compare(path.size() - 4, 4, ".bin") == 0;
}

static void save_db(Db& db, const string& file) {
    if (is_binary_file(file)) {
        if (!write_binary_records(file, collect_records(db)))
            cout << "Ошибка: Не удалось записать бинарный файл " << file << "\n";
        return;
    }
    ofstream out(file);
    if (!out) {
        cout << "Ошибка: Не удалось записать файл " << file << "\n";
        return;
    }
    for (auto& p : db.arrs) {
        out << "M " << p.first;
        int n = p.second->arr_getsz();
        for (int i = 0; i < n; ++i) out << " " << p.second->arr_search(i);
        out << "\n";
    }
    for (auto& p : db.uls) {
        out << "F " << p.first;
        for (auto& kv : p.second->ul_get_pairs()) out << " " << kv.first;
        out << "\n";
    }
    for (auto& p : db.dls) {
        out << "L " << p.first;
        for (auto& v : p.second->dl_items()) out << " " << v;
        out << "\n";
    }
    for (auto& p : db.sts) {
        out << "S " << p.first;
        for (auto& v : p.second->st_items()) out << " " << v;   
        out << "\n";
    }
    for (auto& p : db.qs) {
        out << "Q " << p.first;
        for (auto& v : p.second->q_items()) out << " " << v;
        out << "\n";
    }
    for (auto& p : db.trs) {
        out << "T " << p.first;
        for (int k : p.second->t_items()) out << " " << k;      
        out << "\n";
    }
    for (auto& p : db.cts) {
        out << "C " << p.first;
        for (auto& kv : p.second->ct_get_all()) out << " " << kv.first << " " << kv.second;
        out << "\n";
    }
    for (auto& p : db.cus) {
        out << "K " << p.first;
        for (auto& kv : p.second->cu_get_all()) out << " " << kv.first << " " << kv.second;
        out << "\n";
    }
}

static void load_record(Db& db, const string& type, const string& name,
                        const vector<string>& vals) {
        if (type == "M") {
            Arr* a = get_or_create(db, db.arrs, name, 'M');
            if (a == nullptr) return;
            for (size_t i = 0; i < vals.size(); ++i) a->arr_insert_tail(vals[i]);
        } else if (type == "F") {
            UList* l = get_or_create(db, db.uls, name, 'F');
            if (l == nullptr) return;
            for (size_t i = 0; i < vals.size(); ++i) l->ul_insert_tail(vals[i]);
        } else if (type == "L") {
            DList* l = get_or_create(db, db.dls, name, 'L');
            if (l == nullptr) return;
            for (size_t i = 0; i < vals.size(); ++i) l->dl_insert_tail(vals[i]);
        } else if (type == "S") {
            Stack* s = get_or_create(db, db.sts, name, 'S');
            if (s == nullptr) return;

            for (size_t i = vals.size(); i > 0; --i) s->st_push(vals[i - 1]);
        } else if (type == "Q") {
            Queue* q = get_or_create(db, db.qs, name, 'Q');
            if (q == nullptr) return;
            for (size_t i = 0; i < vals.size(); ++i) q->q_push(vals[i]);
        } else if (type == "T") {
            AVLTree* t = get_or_create(db, db.trs, name, 'T');
            if (t == nullptr) return;
            for (size_t i = 0; i < vals.size(); ++i) {
                int key;
                if (to_key(vals[i], key)) t->t_insert(key);
            }
        } else if (type == "C") {
            ChainTable* h = get_or_create(db, db.cts, name, 'C');
            if (h == nullptr) return;
            if (vals.size() % 2 != 0) {
                cout << "Ошибка: Некорректная запись хеш-таблицы " << name << " в файле\n";
                return;
            }
            for (size_t i = 0; i + 1 < vals.size(); i += 2) h->ct_put(vals[i], vals[i + 1]);
        } else if (type == "K") {
            CuckooTable* h = get_or_create(db, db.cus, name, 'K');
            if (h == nullptr) return;
            if (vals.size() % 2 != 0) {
                cout << "Ошибка: Некорректная запись хеш-таблицы " << name << " в файле\n";
                return;
            }
            for (size_t i = 0; i + 1 < vals.size(); i += 2) h->cu_put(vals[i], vals[i + 1]);
        } else {
            cout << "Ошибка: Неизвестный тип структуры в файле: " << type << "\n";
        }
 }

static bool load_db(Db& db, const string& file) {
    if (is_binary_file(file)) {
        ifstream probe(file, ios::binary);
        if (!probe) return true;  
        probe.close();
        vector<BinaryRecord> records;
        if (!read_binary_records(file, records)) {
            cout << "Ошибка: Повреждённый или неподдерживаемый бинарный файл " << file << "\n";
            return false;
        }
        for (const auto& r : records)
            load_record(db, string(1, r.type), r.name, r.values);
        return true;
    }
    ifstream in(file);
    if (!in) return true;
    string line;
    while (getline(in, line)) {
        istringstream ss(line);
        string type, name, tok;
        if (!(ss >> type >> name)) continue;
        vector<string> vals;
        while (ss >> tok) vals.push_back(tok);
        load_record(db, type, name, vals);
    }
    return true;
}

static void print_any(Db& db, const string& name) {
    auto tp = db.types.find(name);
    if (tp == db.types.end()) {
        cout << "Ошибка: Контейнер " << name << " не найден\n";
        return;
    }
    switch (tp->second) {
        case 'M': db.arrs[name]->arr_print(); break;
        case 'F': db.uls[name]->ul_print_iter(); break;
        case 'L': db.dls[name]->dl_print_iter(); break;
        case 'S': db.sts[name]->st_print(); break;
        case 'Q': db.qs[name]->q_print(); break;
        case 'T': db.trs[name]->t_print_wide(); break;
        case 'C': db.cts[name]->ct_print(); break;
        case 'K': db.cus[name]->cu_print(); break;
    }
}

enum class Cmd {
    Print,
    MCreate, MPush, MInsert, MGet, MDel, MSet, MLen,
    FCreate, FPushHead, FPushTail, FPushBefore, FPushAfter,
    FDelHead, FDelTail, FDelBefore, FDelAfter, FDel, FGet, FPrintRec,
    LCreate, LPushHead, LPushTail, LPushBefore, LPushAfter,
    LDelHead, LDelTail, LDelBefore, LDelAfter, LDel, LGet, LPrintRec,
    SCreate, SPush, SPop,
    QCreate, QPush, QPop,
    TCreate, TInsert, TDel, TGet,
    CCreate, CPut, CGet, CDel, CLen,
    KCreate, KPut, KGet, KDel, KLen
};

struct CmdInfo {
    Cmd cmd;
    size_t argc;
    bool changes;
};

static const CmdInfo* find_cmd(const string& name) {
    static const map<string, CmdInfo> table = {
        {"PRINT", {Cmd::Print, 2, false}},

        {"MCREATE", {Cmd::MCreate, 2, true}},
        {"MPUSH", {Cmd::MPush, 3, true}},
        {"MINSERT", {Cmd::MInsert, 4, true}},
        {"MGET", {Cmd::MGet, 3, false}},
        {"MDEL", {Cmd::MDel, 3, true}},
        {"MSET", {Cmd::MSet, 4, true}},
        {"MLEN", {Cmd::MLen, 2, false}},

        {"FCREATE", {Cmd::FCreate, 2, true}},
        {"FPUSHHEAD", {Cmd::FPushHead, 3, true}},
        {"FPUSHTAIL", {Cmd::FPushTail, 3, true}},
        {"FPUSHBEFORE", {Cmd::FPushBefore, 4, true}},
        {"FPUSHAFTER", {Cmd::FPushAfter, 4, true}},
        {"FDELHEAD", {Cmd::FDelHead, 2, true}},
        {"FDELTAIL", {Cmd::FDelTail, 2, true}},
        {"FDELBEFORE", {Cmd::FDelBefore, 3, true}},
        {"FDELAFTER", {Cmd::FDelAfter, 3, true}},
        {"FDEL", {Cmd::FDel, 3, true}},
        {"FGET", {Cmd::FGet, 3, false}},
        {"FPRINTREC", {Cmd::FPrintRec, 2, false}},

        {"LCREATE", {Cmd::LCreate, 2, true}},
        {"LPUSHHEAD", {Cmd::LPushHead, 3, true}},
        {"LPUSHTAIL", {Cmd::LPushTail, 3, true}},
        {"LPUSHBEFORE", {Cmd::LPushBefore, 4, true}},
        {"LPUSHAFTER", {Cmd::LPushAfter, 4, true}},
        {"LDELHEAD", {Cmd::LDelHead, 2, true}},
        {"LDELTAIL", {Cmd::LDelTail, 2, true}},
        {"LDELBEFORE", {Cmd::LDelBefore, 3, true}},
        {"LDELAFTER", {Cmd::LDelAfter, 3, true}},
        {"LDEL", {Cmd::LDel, 3, true}},
        {"LGET", {Cmd::LGet, 3, false}},
        {"LPRINTREC", {Cmd::LPrintRec, 2, false}},

        {"SCREATE", {Cmd::SCreate, 2, true}},
        {"SPUSH", {Cmd::SPush, 3, true}},
        {"SPOP", {Cmd::SPop, 2, true}},

        {"QCREATE", {Cmd::QCreate, 2, true}},
        {"QPUSH", {Cmd::QPush, 3, true}},
        {"QPOP", {Cmd::QPop, 2, true}},

        {"TCREATE", {Cmd::TCreate, 2, true}},
        {"TINSERT", {Cmd::TInsert, 3, true}},
        {"TDEL", {Cmd::TDel, 3, true}},
        {"TGET", {Cmd::TGet, 3, false}},
        {"ISMEMBER", {Cmd::TGet, 3, false}},

        {"CCREATE", {Cmd::CCreate, 2, true}},
        {"CPUT", {Cmd::CPut, 4, true}},
        {"CGET", {Cmd::CGet, 3, false}},
        {"CDEL", {Cmd::CDel, 3, true}},
        {"CLEN", {Cmd::CLen, 2, false}},

        {"KCREATE", {Cmd::KCreate, 2, true}},
        {"KPUT", {Cmd::KPut, 4, true}},
        {"KGET", {Cmd::KGet, 3, false}},
        {"KDEL", {Cmd::KDel, 3, true}},
        {"KLEN", {Cmd::KLen, 2, false}},
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

        case Cmd::MCreate:
            get_or_create(db, db.arrs, t[1], 'M');
            break;
        case Cmd::MPush: {
            Arr* a = get_or_create(db, db.arrs, t[1], 'M');
            if (a) a->arr_insert_tail(t[2]);
            break;
        }
        case Cmd::MInsert: {
            int idx;
            Arr* a = find_cont(db.arrs, t[1]);
            if (a && to_int(t[2], idx)) a->arr_insert_idx(t[3], idx);
            break;
        }
        case Cmd::MGet: {
            int idx;
            Arr* a = find_cont(db.arrs, t[1]);
            if (a && to_int(t[2], idx)) {
                string v = a->arr_search(idx);
                if (idx >= 0 && idx < a->arr_getsz()) cout << v << endl;
            }
            break;
        }
        case Cmd::MDel: {
            int idx;
            Arr* a = find_cont(db.arrs, t[1]);
            if (a && to_int(t[2], idx)) a->arr_delete_idx(idx);
            break;
        }
        case Cmd::MSet: {
            int idx;
            Arr* a = find_cont(db.arrs, t[1]);
            if (a && to_int(t[2], idx)) a->arr_change_idx(t[3], idx);
            break;
        }
        case Cmd::MLen: {
            Arr* a = find_cont(db.arrs, t[1]);
            if (a) cout << a->arr_getsz() << endl;
            break;
        }

        case Cmd::FCreate:
            get_or_create(db, db.uls, t[1], 'F');
            break;
        case Cmd::FPushHead: {
            UList* l = get_or_create(db, db.uls, t[1], 'F');
            if (l) l->ul_insert_head(t[2]);
            break;
        }
        case Cmd::FPushTail: {
            UList* l = get_or_create(db, db.uls, t[1], 'F');
            if (l) l->ul_insert_tail(t[2]);
            break;
        }
        case Cmd::FPushBefore: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_insert_before(t[2], t[3]);
            break;
        }
        case Cmd::FPushAfter: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_insert_after(t[2], t[3]);
            break;
        }
        case Cmd::FDelHead: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_delete_head();
            break;
        }
        case Cmd::FDelTail: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_delete_tail();
            break;
        }
        case Cmd::FDelBefore: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_delete_before(t[2]);
            break;
        }
        case Cmd::FDelAfter: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_delete_after(t[2]);
            break;
        }
        case Cmd::FDel: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_delete_key(t[2]);
            break;
        }
        case Cmd::FGet: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_search(t[2]);
            break;
        }
        case Cmd::FPrintRec: {
            UList* l = find_cont(db.uls, t[1]);
            if (l) l->ul_print_rec();
            break;
        }

        case Cmd::LCreate:
            get_or_create(db, db.dls, t[1], 'L');
            break;
        case Cmd::LPushHead: {
            DList* l = get_or_create(db, db.dls, t[1], 'L');
            if (l) l->dl_insert_head(t[2]);
            break;
        }
        case Cmd::LPushTail: {
            DList* l = get_or_create(db, db.dls, t[1], 'L');
            if (l) l->dl_insert_tail(t[2]);
            break;
        }
        case Cmd::LPushBefore: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_insert_before(t[2], t[3]);
            break;
        }
        case Cmd::LPushAfter: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_insert_after(t[2], t[3]);
            break;
        }
        case Cmd::LDelHead: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_delete_head();
            break;
        }
        case Cmd::LDelTail: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_delete_tail();
            break;
        }
        case Cmd::LDelBefore: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_delete_before(t[2]);
            break;
        }
        case Cmd::LDelAfter: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_delete_after(t[2]);
            break;
        }
        case Cmd::LDel: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_delete_key(t[2]);
            break;
        }
        case Cmd::LGet: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_search(t[2]);
            break;
        }
        case Cmd::LPrintRec: {
            DList* l = find_cont(db.dls, t[1]);
            if (l) l->dl_print_rec();
            break;
        }

        case Cmd::SCreate:
            get_or_create(db, db.sts, t[1], 'S');
            break;
        case Cmd::SPush: {
            Stack* s = get_or_create(db, db.sts, t[1], 'S');
            if (s) {
                s->st_push(t[2]);
                cout << t[2] << endl;
            }
            break;
        }
        case Cmd::SPop: {
            Stack* s = find_cont(db.sts, t[1]);
            if (s) s->st_pop();
            break;
        }

        case Cmd::QCreate:
            get_or_create(db, db.qs, t[1], 'Q');
            break;
        case Cmd::QPush: {
            Queue* q = get_or_create(db, db.qs, t[1], 'Q');
            if (q) {
                q->q_push(t[2]);
                cout << t[2] << endl;
            }
            break;
        }
        case Cmd::QPop: {
            Queue* q = find_cont(db.qs, t[1]);
            if (q) q->q_pop();
            break;
        }

        case Cmd::TCreate:
            get_or_create(db, db.trs, t[1], 'T');
            break;
        case Cmd::TInsert: {
            AVLTree* tr = get_or_create(db, db.trs, t[1], 'T');
            int key;
            if (tr && to_key(t[2], key)) tr->t_insert(key);
            break;
        }
        case Cmd::TDel: {
            AVLTree* tr = find_cont(db.trs, t[1]);
            int key;
            if (tr && to_key(t[2], key)) tr->t_delete(key);
            break;
        }
        case Cmd::TGet: {
            AVLTree* tr = find_cont(db.trs, t[1]);
            int key;
            if (tr && to_key(t[2], key)) cout << (tr->t_search(key) ? "TRUE" : "FALSE") << endl;
            break;
        }

        case Cmd::CCreate:
            get_or_create(db, db.cts, t[1], 'C');
            break;
        case Cmd::CPut: {
            ChainTable* h = get_or_create(db, db.cts, t[1], 'C');
            if (h) h->ct_put(t[2], t[3]);
            break;
        }
        case Cmd::CGet: {
            ChainTable* h = find_cont(db.cts, t[1]);
            string v;
            if (h) {
                if (h->ct_get(t[2], v)) cout << v << endl;
                else cout << "Ошибка: Не найдено\n";
            }
            break;
        }
        case Cmd::CDel: {
            ChainTable* h = find_cont(db.cts, t[1]);
            if (h && !h->ct_remove(t[2])) cout << "Ошибка: Не найдено\n";
            break;
        }
        case Cmd::CLen: {
            ChainTable* h = find_cont(db.cts, t[1]);
            if (h) cout << h->ct_getsz() << endl;
            break;
        }

        case Cmd::KCreate:
            get_or_create(db, db.cus, t[1], 'K');
            break;
        case Cmd::KPut: {
            CuckooTable* h = get_or_create(db, db.cus, t[1], 'K');
            if (h) h->cu_put(t[2], t[3]);
            break;
        }
        case Cmd::KGet: {
            CuckooTable* h = find_cont(db.cus, t[1]);
            string v;
            if (h) {
                if (h->cu_get(t[2], v)) cout << v << endl;
                else cout << "Ошибка: Не найдено\n";
            }
            break;
        }
        case Cmd::KDel: {
            CuckooTable* h = find_cont(db.cus, t[1]);
            if (h && !h->cu_remove(t[2])) cout << "Ошибка: Не найдено\n";
            break;
        }
        case Cmd::KLen: {
            CuckooTable* h = find_cont(db.cus, t[1]);
            if (h) cout << h->cu_getsz() << endl;
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
        cout << "Использование: ./dbms --file file.data|file.bin --query 'КОМАНДА имя [аргументы]'\n";
        return 1;
    }

    vector<string> t;
    istringstream ss(query);
    string tok;
    while (ss >> std::quoted(tok)) t.push_back(tok);
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
    if (t.size() != info->argc) {
        cout << "Ошибка: Неверное число аргументов для " << t[0] << "\n";
        return 1;
    }

    Db db;
    if (!load_db(db, file)) return 1;
    run(db, info->cmd, t);

    if (info->changes) save_db(db, file);

    return 0;
}
