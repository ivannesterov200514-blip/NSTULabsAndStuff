use std::collections::BTreeMap;
use std::env;
use std::fs;
use std::process;

use lab3_rust::array::Arr;
use lab3_rust::avltree::AVLTree;
use lab3_rust::dlist::DList;
use lab3_rust::hashtable::{ChainTable, CuckooTable};
use lab3_rust::queue::Queue;
use lab3_rust::stack::Stack;
use lab3_rust::ulist::UList;

// Типы структур (символ в файле):
// M - массив, F - односвязный список, L - двусвязный список, S - стек,
// Q - очередь, T - AVL-дерево, C - хеш-таблица (цепочки), K - хеш-таблица (кукушка)
#[derive(Default)]
struct Db {
    arrs: BTreeMap<String, Arr>,
    uls: BTreeMap<String, UList>,
    dls: BTreeMap<String, DList>,
    sts: BTreeMap<String, Stack>,
    qs: BTreeMap<String, Queue>,
    trs: BTreeMap<String, AVLTree>,
    cts: BTreeMap<String, ChainTable>,
    cus: BTreeMap<String, CuckooTable>,
    types: BTreeMap<String, char>,
}

fn find_cont<'a, T>(map: &'a mut BTreeMap<String, T>, name: &str) -> Option<&'a mut T> {
    let found = map.get_mut(name);
    if found.is_none() {
        println!("Ошибка: Контейнер {} не найден", name);
    }
    found
}

fn get_or_create<'a, T, F: FnOnce() -> T>(
    types: &mut BTreeMap<String, char>,
    map: &'a mut BTreeMap<String, T>,
    name: &str,
    ty: char,
    make: F,
) -> Option<&'a mut T> {
    match types.get(name) {
        Some(&t) if t != ty => {
            println!("Ошибка: Имя {} уже занято другой структурой", name);
            return None;
        }
        Some(_) => {}
        None => {
            types.insert(name.to_string(), ty);
        }
    }
    Some(map.entry(name.to_string()).or_insert_with(make))
}

macro_rules! goc {
    ($db:expr, $map:ident, $name:expr, $ty:expr, $make:expr) => {
        get_or_create(&mut $db.types, &mut $db.$map, $name, $ty, $make)
    };
}

macro_rules! fc {
    ($db:expr, $map:ident, $name:expr) => {
        find_cont(&mut $db.$map, $name)
    };
}

fn to_int(s: &str) -> Option<isize> {
    let r = s.parse::<isize>().ok();
    if r.is_none() {
        println!("Ошибка: Индекс должен быть числом");
    }
    r
}

fn to_key(s: &str) -> Option<i32> {
    let r = s.parse::<i32>().ok();
    if r.is_none() {
        println!("Ошибка: Ключ дерева должен быть целым числом");
    }
    r
}

fn push_line(out: &mut String, ty: char, name: &str, vals: &[String]) {
    out.push(ty);
    out.push(' ');
    out.push_str(name);
    for v in vals {
        out.push(' ');
        out.push_str(v);
    }
    out.push('\n');
}

fn flatten_pairs(pairs: Vec<(String, String)>) -> Vec<String> {
    let mut res = Vec::with_capacity(pairs.len() * 2);
    for (k, v) in pairs {
        res.push(k);
        res.push(v);
    }
    res
}

fn save_db(db: &Db, file: &str) {
    let mut out = String::new();
    for (name, a) in &db.arrs {
        push_line(&mut out, 'M', name, &a.items());
    }
    for (name, l) in &db.uls {
        let keys: Vec<String> = l.get_pairs().into_iter().map(|p| p.0).collect();
        push_line(&mut out, 'F', name, &keys);
    }
    for (name, l) in &db.dls {
        push_line(&mut out, 'L', name, &l.items());
    }
    for (name, s) in &db.sts {
        push_line(&mut out, 'S', name, &s.items()); // от вершины ко дну
    }
    for (name, q) in &db.qs {
        push_line(&mut out, 'Q', name, &q.items());
    }
    for (name, t) in &db.trs {
        let keys: Vec<String> = t.items().iter().map(|k| k.to_string()).collect();
        push_line(&mut out, 'T', name, &keys); // обход по уровням
    }
    for (name, h) in &db.cts {
        push_line(&mut out, 'C', name, &flatten_pairs(h.get_all()));
    }
    for (name, h) in &db.cus {
        push_line(&mut out, 'K', name, &flatten_pairs(h.get_all()));
    }
    if fs::write(file, out).is_err() {
        println!("Ошибка: Не удалось записать файл {}", file);
    }
}

fn load_db(db: &mut Db, file: &str) {
    let text = match fs::read_to_string(file) {
        Ok(t) => t,
        Err(_) => return,
    };
    for line in text.lines() {
        let mut it = line.split_whitespace();
        let (ty, name) = match (it.next(), it.next()) {
            (Some(a), Some(b)) => (a, b),
            _ => continue,
        };
        let vals: Vec<&str> = it.collect();

        match ty {
            "M" => {
                if let Some(a) = goc!(db, arrs, name, 'M', Arr::default) {
                    for v in &vals {
                        a.insert_tail(v.to_string());
                    }
                }
            }
            "F" => {
                if let Some(l) = goc!(db, uls, name, 'F', UList::default) {
                    for v in &vals {
                        l.insert_tail(v.to_string());
                    }
                }
            }
            "L" => {
                if let Some(l) = goc!(db, dls, name, 'L', DList::default) {
                    for v in &vals {
                        l.insert_tail(v.to_string());
                    }
                }
            }
            "S" => {
                if let Some(s) = goc!(db, sts, name, 'S', Stack::default) {
                    // в файле порядок от вершины ко дну, поэтому кладем с конца
                    for v in vals.iter().rev() {
                        s.push(v.to_string());
                    }
                }
            }
            "Q" => {
                if let Some(q) = goc!(db, qs, name, 'Q', Queue::default) {
                    for v in &vals {
                        q.push(v.to_string());
                    }
                }
            }
            "T" => {
                if let Some(t) = goc!(db, trs, name, 'T', AVLTree::default) {
                    for v in &vals {
                        if let Some(k) = to_key(v) {
                            t.insert(k);
                        }
                    }
                }
            }
            "C" => {
                if let Some(h) = goc!(db, cts, name, 'C', ChainTable::default) {
                    if vals.len() % 2 != 0 {
                        println!("Ошибка: Некорректная запись хеш-таблицы {} в файле", name);
                        continue;
                    }
                    for p in vals.chunks(2) {
                        h.put(p[0].to_string(), p[1].to_string());
                    }
                }
            }
            "K" => {
                if let Some(h) = goc!(db, cus, name, 'K', CuckooTable::default) {
                    if vals.len() % 2 != 0 {
                        println!("Ошибка: Некорректная запись хеш-таблицы {} в файле", name);
                        continue;
                    }
                    for p in vals.chunks(2) {
                        h.put(p[0].to_string(), p[1].to_string());
                    }
                }
            }
            other => println!("Ошибка: Неизвестный тип структуры в файле: {}", other),
        }
    }
}

fn print_any(db: &Db, name: &str) {
    match db.types.get(name) {
        None => println!("Ошибка: Контейнер {} не найден", name),
        Some('M') => db.arrs[name].print(),
        Some('F') => db.uls[name].print_iter(),
        Some('L') => db.dls[name].print_iter(),
        Some('S') => db.sts[name].print(),
        Some('Q') => db.qs[name].print(),
        Some('T') => db.trs[name].print_wide(),
        Some('C') => db.cts[name].print(),
        Some('K') => db.cus[name].print(),
        Some(_) => {}
    }
}

/// (число токенов команды вместе с ее именем, меняет ли команда данные)
fn cmd_info(name: &str) -> Option<(usize, bool)> {
    Some(match name {
        "PRINT" => (2, false),

        "MCREATE" => (2, true),
        "MPUSH" => (3, true),
        "MINSERT" => (4, true),
        "MGET" => (3, false),
        "MDEL" => (3, true),
        "MSET" => (4, true),
        "MLEN" => (2, false),

        "FCREATE" | "LCREATE" => (2, true),
        "FPUSHHEAD" | "FPUSHTAIL" | "LPUSHHEAD" | "LPUSHTAIL" => (3, true),
        "FPUSHBEFORE" | "FPUSHAFTER" | "LPUSHBEFORE" | "LPUSHAFTER" => (4, true),
        "FDELHEAD" | "FDELTAIL" | "LDELHEAD" | "LDELTAIL" => (2, true),
        "FDELBEFORE" | "FDELAFTER" | "FDEL" | "LDELBEFORE" | "LDELAFTER" | "LDEL" => (3, true),
        "FGET" | "LGET" => (3, false),
        "FPRINTREC" | "LPRINTREC" => (2, false),

        "SCREATE" | "QCREATE" => (2, true),
        "SPUSH" | "QPUSH" => (3, true),
        "SPOP" | "QPOP" => (2, true),

        "TCREATE" => (2, true),
        "TINSERT" | "TDEL" => (3, true),
        "TGET" | "ISMEMBER" => (3, false),

        "CCREATE" | "KCREATE" => (2, true),
        "CPUT" | "KPUT" => (4, true),
        "CGET" | "KGET" => (3, false),
        "CDEL" | "KDEL" => (3, true),
        "CLEN" | "KLEN" => (2, false),

        _ => return None,
    })
}

fn run(db: &mut Db, t: &[String]) {
    let name = t[1].as_str();
    match t[0].as_str() {
        "PRINT" => print_any(db, name),

        // ---------- массив ----------
        "MCREATE" => {
            goc!(db, arrs, name, 'M', Arr::default);
        }
        "MPUSH" => {
            if let Some(a) = goc!(db, arrs, name, 'M', Arr::default) {
                a.insert_tail(t[2].clone());
            }
        }
        "MINSERT" => {
            if let Some(a) = fc!(db, arrs, name) {
                if let Some(idx) = to_int(&t[2]) {
                    a.insert_idx(t[3].clone(), idx);
                }
            }
        }
        "MGET" => {
            if let Some(a) = fc!(db, arrs, name) {
                if let Some(idx) = to_int(&t[2]) {
                    if let Some(v) = a.search(idx) {
                        println!("{}", v);
                    }
                }
            }
        }
        "MDEL" => {
            if let Some(a) = fc!(db, arrs, name) {
                if let Some(idx) = to_int(&t[2]) {
                    a.delete_idx(idx);
                }
            }
        }
        "MSET" => {
            if let Some(a) = fc!(db, arrs, name) {
                if let Some(idx) = to_int(&t[2]) {
                    a.change_idx(t[3].clone(), idx);
                }
            }
        }
        "MLEN" => {
            if let Some(a) = fc!(db, arrs, name) {
                println!("{}", a.size());
            }
        }

        // ---------- односвязный список ----------
        "FCREATE" => {
            goc!(db, uls, name, 'F', UList::default);
        }
        "FPUSHHEAD" => {
            if let Some(l) = goc!(db, uls, name, 'F', UList::default) {
                l.insert_head(t[2].clone());
            }
        }
        "FPUSHTAIL" => {
            if let Some(l) = goc!(db, uls, name, 'F', UList::default) {
                l.insert_tail(t[2].clone());
            }
        }
        "FPUSHBEFORE" => {
            if let Some(l) = fc!(db, uls, name) {
                l.insert_before(&t[2], t[3].clone());
            }
        }
        "FPUSHAFTER" => {
            if let Some(l) = fc!(db, uls, name) {
                l.insert_after(&t[2], t[3].clone());
            }
        }
        "FDELHEAD" => {
            if let Some(l) = fc!(db, uls, name) {
                l.delete_head();
            }
        }
        "FDELTAIL" => {
            if let Some(l) = fc!(db, uls, name) {
                l.delete_tail();
            }
        }
        "FDELBEFORE" => {
            if let Some(l) = fc!(db, uls, name) {
                l.delete_before(&t[2]);
            }
        }
        "FDELAFTER" => {
            if let Some(l) = fc!(db, uls, name) {
                l.delete_after(&t[2]);
            }
        }
        "FDEL" => {
            if let Some(l) = fc!(db, uls, name) {
                l.delete_key(&t[2]);
            }
        }
        "FGET" => {
            if let Some(l) = fc!(db, uls, name) {
                l.search(&t[2]);
            }
        }
        "FPRINTREC" => {
            if let Some(l) = fc!(db, uls, name) {
                l.print_rec();
            }
        }

        // ---------- двусвязный список ----------
        "LCREATE" => {
            goc!(db, dls, name, 'L', DList::default);
        }
        "LPUSHHEAD" => {
            if let Some(l) = goc!(db, dls, name, 'L', DList::default) {
                l.insert_head(t[2].clone());
            }
        }
        "LPUSHTAIL" => {
            if let Some(l) = goc!(db, dls, name, 'L', DList::default) {
                l.insert_tail(t[2].clone());
            }
        }
        "LPUSHBEFORE" => {
            if let Some(l) = fc!(db, dls, name) {
                l.insert_before(&t[2], t[3].clone());
            }
        }
        "LPUSHAFTER" => {
            if let Some(l) = fc!(db, dls, name) {
                l.insert_after(&t[2], t[3].clone());
            }
        }
        "LDELHEAD" => {
            if let Some(l) = fc!(db, dls, name) {
                l.delete_head();
            }
        }
        "LDELTAIL" => {
            if let Some(l) = fc!(db, dls, name) {
                l.delete_tail();
            }
        }
        "LDELBEFORE" => {
            if let Some(l) = fc!(db, dls, name) {
                l.delete_before(&t[2]);
            }
        }
        "LDELAFTER" => {
            if let Some(l) = fc!(db, dls, name) {
                l.delete_after(&t[2]);
            }
        }
        "LDEL" => {
            if let Some(l) = fc!(db, dls, name) {
                l.delete_key(&t[2]);
            }
        }
        "LGET" => {
            if let Some(l) = fc!(db, dls, name) {
                l.search(&t[2]);
            }
        }
        "LPRINTREC" => {
            if let Some(l) = fc!(db, dls, name) {
                l.print_rec();
            }
        }

        // ---------- стек ----------
        "SCREATE" => {
            goc!(db, sts, name, 'S', Stack::default);
        }
        "SPUSH" => {
            if let Some(s) = goc!(db, sts, name, 'S', Stack::default) {
                s.push(t[2].clone());
                println!("{}", t[2]);
            }
        }
        "SPOP" => {
            if let Some(s) = fc!(db, sts, name) {
                s.pop();
            }
        }

        // ---------- очередь ----------
        "QCREATE" => {
            goc!(db, qs, name, 'Q', Queue::default);
        }
        "QPUSH" => {
            if let Some(q) = goc!(db, qs, name, 'Q', Queue::default) {
                q.push(t[2].clone());
                println!("{}", t[2]);
            }
        }
        "QPOP" => {
            if let Some(q) = fc!(db, qs, name) {
                q.pop();
            }
        }

        // ---------- AVL-дерево ----------
        "TCREATE" => {
            goc!(db, trs, name, 'T', AVLTree::default);
        }
        "TINSERT" => {
            if let Some(tr) = goc!(db, trs, name, 'T', AVLTree::default) {
                if let Some(k) = to_key(&t[2]) {
                    tr.insert(k);
                }
            }
        }
        "TDEL" => {
            if let Some(tr) = fc!(db, trs, name) {
                if let Some(k) = to_key(&t[2]) {
                    tr.delete(k);
                }
            }
        }
        "TGET" | "ISMEMBER" => {
            if let Some(tr) = fc!(db, trs, name) {
                if let Some(k) = to_key(&t[2]) {
                    println!("{}", if tr.search(k) { "TRUE" } else { "FALSE" });
                }
            }
        }

        // ---------- хеш-таблица (цепочки) ----------
        "CCREATE" => {
            goc!(db, cts, name, 'C', ChainTable::default);
        }
        "CPUT" => {
            if let Some(h) = goc!(db, cts, name, 'C', ChainTable::default) {
                h.put(t[2].clone(), t[3].clone());
            }
        }
        "CGET" => {
            if let Some(h) = fc!(db, cts, name) {
                match h.get(&t[2]) {
                    Some(v) => println!("{}", v),
                    None => println!("Ошибка: Не найдено"),
                }
            }
        }
        "CDEL" => {
            if let Some(h) = fc!(db, cts, name) {
                if !h.remove(&t[2]) {
                    println!("Ошибка: Не найдено");
                }
            }
        }
        "CLEN" => {
            if let Some(h) = fc!(db, cts, name) {
                println!("{}", h.size());
            }
        }

        // ---------- хеш-таблица (кукушка) ----------
        "KCREATE" => {
            goc!(db, cus, name, 'K', CuckooTable::default);
        }
        "KPUT" => {
            if let Some(h) = goc!(db, cus, name, 'K', CuckooTable::default) {
                h.put(t[2].clone(), t[3].clone());
            }
        }
        "KGET" => {
            if let Some(h) = fc!(db, cus, name) {
                match h.get(&t[2]) {
                    Some(v) => println!("{}", v),
                    None => println!("Ошибка: Не найдено"),
                }
            }
        }
        "KDEL" => {
            if let Some(h) = fc!(db, cus, name) {
                if !h.remove(&t[2]) {
                    println!("Ошибка: Не найдено");
                }
            }
        }
        "KLEN" => {
            if let Some(h) = fc!(db, cus, name) {
                println!("{}", h.size());
            }
        }

        _ => {}
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut file = String::new();
    let mut query = String::new();
    let mut i = 1;
    while i < args.len() {
        if args[i] == "--file" && i + 1 < args.len() {
            file = args[i + 1].clone();
            i += 2;
        } else if args[i] == "--query" && i + 1 < args.len() {
            query = args[i + 1].clone();
            i += 2;
        } else {
            i += 1;
        }
    }
    if file.is_empty() || query.is_empty() {
        println!("Использование: dbms --file file.data --query 'КОМАНДА имя [аргументы]'");
        process::exit(1);
    }

    let mut t: Vec<String> = query.split_whitespace().map(String::from).collect();
    if t.is_empty() {
        println!("Ошибка: Пустой запрос");
        process::exit(1);
    }
    t[0] = t[0].to_uppercase();

    let (argc, changes) = match cmd_info(&t[0]) {
        Some(info) => info,
        None => {
            println!("Ошибка: Неизвестная команда {}", t[0]);
            process::exit(1);
        }
    };
    if t.len() != argc {
        println!("Ошибка: Неверное число аргументов для {}", t[0]);
        process::exit(1);
    }

    let mut db = Db::default();
    load_db(&mut db, &file);
    run(&mut db, &t);

    if changes {
        save_db(&db, &file);
    }
}
