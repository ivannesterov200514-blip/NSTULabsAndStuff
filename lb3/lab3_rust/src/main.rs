use std::collections::BTreeMap;
use std::env;
use std::io::ErrorKind;
use std::process;

use lab3_rust::array::Arr;
use lab3_rust::avltree::AVLTree;
use lab3_rust::dlist::DList;
use lab3_rust::hashtable::{ChainTable, CuckooTable};
use lab3_rust::queue::Queue;
use lab3_rust::stack::Stack;
use lab3_rust::ulist::UList;
use lab3_rust::storage::{Record, load_records, save_records};

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

fn flatten_pairs(pairs: Vec<(String, String)>) -> Vec<String> {
    let mut result = Vec::new();
    for (key, value) in pairs {
        result.push(key);
        result.push(value);
    }
    result
}

fn collect_records(db: &Db) -> Vec<Record> {
    let mut records = Vec::new();
    for (name, a) in &db.arrs {
        records.push(Record { kind: 'M', name: name.clone(), values: a.items() });
    }
    for (name, l) in &db.uls {
        let values = l.get_pairs().into_iter().map(|p| p.0).collect();
        records.push(Record { kind: 'F', name: name.clone(), values });
    }
    for (name, l) in &db.dls {
        records.push(Record { kind: 'L', name: name.clone(), values: l.items() });
    }
    for (name, a) in &db.sts {
        records.push(Record { kind: 'S', name: name.clone(), values: a.items() });
    }
    for (name, a) in &db.qs {
        records.push(Record { kind: 'Q', name: name.clone(), values: a.items() });
    }
    for (name, a) in &db.trs {
        let values = a.items().iter().map(|v| v.to_string()).collect();
        records.push(Record { kind: 'T', name: name.clone(), values });
    }
    for (name, a) in &db.cts {
        records.push(Record { kind: 'C', name: name.clone(), values: flatten_pairs(a.get_all()) });
    }
    for (name, a) in &db.cus {
        records.push(Record { kind: 'K', name: name.clone(), values: flatten_pairs(a.get_all()) });
    }
    records
}

fn save_db(db: &Db, file: &str) -> bool {
    match save_records(file, &collect_records(db)) {
        Ok(()) => true,
        Err(e) => {
            println!("Ошибка: Не удалось сохранить файл {}: {}", file, e);
            false
        }
    }
}

fn load_db(db: &mut Db, file: &str) -> bool {
    let records = match load_records(file) {
        Ok(records) => records,
        Err(e) if e.kind() == ErrorKind::NotFound => return true,
        Err(e) => {
            println!("Ошибка: Не удалось прочитать файл {}: {}", file, e);
            return false;
        }
    };
    for record in records {
        let name = record.name.as_str();
        let values: Vec<&str> = record.values.iter().map(String::as_str).collect();
        match record.kind {
            'M' => {
                if let Some(a) = goc!(db, arrs, name, 'M', Arr::default) {
                    for value in &values { a.insert_tail(value.to_string()); }
                }
            }
            'F' => {
                if let Some(a) = goc!(db, uls, name, 'F', UList::default) {
                    for value in &values { a.insert_tail(value.to_string()); }
                }
            }
            'L' => {
                if let Some(a) = goc!(db, dls, name, 'L', DList::default) {
                    for value in &values { a.insert_tail(value.to_string()); }
                }
            }
            'S' => {
                if let Some(a) = goc!(db, sts, name, 'S', Stack::default) {
                    for value in values.iter().rev() { a.push(value.to_string()); }
                }
            }
            'Q' => {
                if let Some(a) = goc!(db, qs, name, 'Q', Queue::default) {
                    for value in &values { a.push(value.to_string()); }
                }
            }
            'T' => {
                if let Some(a) = goc!(db, trs, name, 'T', AVLTree::default) {
                    for value in &values {
                        if let Some(key) = to_key(value) { a.insert(key); }
                    }
                }
            }
            'C' => {
                if values.len() % 2 != 0 {
                    println!("Ошибка: Некорректная запись хеш-таблицы {}", name);
                    return false;
                }
                if let Some(a) = goc!(db, cts, name, 'C', ChainTable::default) {
                    for pair in values.chunks(2) { a.put(pair[0].to_string(), pair[1].to_string()); }
                }
            }
            'K' => {
                if values.len() % 2 != 0 {
                    println!("Ошибка: Некорректная запись хеш-таблицы {}", name);
                    return false;
                }
                if let Some(a) = goc!(db, cus, name, 'K', CuckooTable::default) {
                    for pair in values.chunks(2) { a.put(pair[0].to_string(), pair[1].to_string()); }
                }
            }
            other => {
                println!("Ошибка: Неизвестный тип структуры в файле: {}", other);
                return false;
            }
        }
    }
    true
}

fn parse_query(query: &str) -> Option<Vec<String>> {
    let mut words = Vec::new();
    let mut word = String::new();
    let mut quoted = false;
    let mut escaped = false;
    let mut started = false;
    for ch in query.chars() {
        if escaped {
            word.push(ch);
            escaped = false;
        } else if ch == '\\' && quoted {
            escaped = true;
        } else if ch == '"' {
            quoted = !quoted;
            started = true;
        } else if ch.is_whitespace() && !quoted {
            if started {
                words.push(std::mem::take(&mut word));
                started = false;
            }
        } else {
            word.push(ch);
            started = true;
        }
    }
    if quoted || escaped {
        println!("Ошибка: Незакрытые кавычки в запросе");
        return None;
    }
    if started { words.push(word); }
    Some(words)
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
        println!("Использование: dbms --file file.data|file.bin --query 'КОМАНДА имя [аргументы]'");
        process::exit(1);
    }

    let mut t = match parse_query(&query) {
        Some(words) => words,
        None => process::exit(1),
    };
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
    if !load_db(&mut db, &file) {
        process::exit(1);
    }
    run(&mut db, &t);

    if changes {
        if !save_db(&db, &file) {
            process::exit(1);
        }
    }
}