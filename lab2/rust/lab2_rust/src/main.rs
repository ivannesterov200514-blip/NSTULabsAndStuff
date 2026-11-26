mod array;
mod asteroids;
mod hashtable;
mod lfu;
mod set;
mod ulist;
mod zmeika;

use array::Arr;
use hashtable::{genealogy, ChainTable, CuckooTable};
use lfu::Lfu;
use set::{turtles, Set};
use std::collections::BTreeMap;
use std::env;
use std::fs;
use zmeika::AvlTree;

enum Container {
    Set(Set),
    Tree(AvlTree),
    Chain(ChainTable),
    Cuckoo(CuckooTable),
    Lfu(Lfu),
}

impl Container {
    fn kind(&self) -> char {
        match self {
            Container::Set(_) => 'H',
            Container::Tree(_) => 'T',
            Container::Chain(_) => 'C',
            Container::Cuckoo(_) => 'K',
            Container::Lfu(_) => 'U',
        }
    }
}

#[derive(Default)]
struct Db {
    containers: BTreeMap<String, Container>,
}

impl Db {
    fn ensure(&mut self, name: &str, kind: char) -> bool {
        if let Some(existing) = self.containers.get(name) {
            if existing.kind() != kind {
                println!("Ошибка: Имя {} уже занято другой структурой", name);
                return false;
            }
            return true;
        }
        let container = match kind {
            'H' => Container::Set(Set::new()),
            'T' => Container::Tree(AvlTree::default()),
            'C' => Container::Chain(ChainTable::default()),
            'K' => Container::Cuckoo(CuckooTable::default()),
            'U' => Container::Lfu(Lfu::default()),
            _ => return false,
        };
        self.containers.insert(name.to_string(), container);
        true
    }

    fn get_set_mut(&mut self, name: &str) -> Option<&mut Set> {
        match self.containers.get_mut(name) {
            Some(Container::Set(value)) => Some(value),
            _ => {
                println!("Ошибка: Контейнер {} не найден", name);
                None
            }
        }
    }

    fn get_tree_mut(&mut self, name: &str) -> Option<&mut AvlTree> {
        match self.containers.get_mut(name) {
            Some(Container::Tree(value)) => Some(value),
            _ => {
                println!("Ошибка: Контейнер {} не найден", name);
                None
            }
        }
    }

    fn get_lfu_mut(&mut self, name: &str) -> Option<&mut Lfu> {
        match self.containers.get_mut(name) {
            Some(Container::Lfu(value)) => Some(value),
            _ => {
                println!("Ошибка: Контейнер {} не найден", name);
                None
            }
        }
    }

    fn hash_kind(&self, name: &str) -> char {
        match self.containers.get(name) {
            None => {
                println!("Ошибка: Контейнер {} не найден", name);
                '\0'
            }
            Some(Container::Chain(_)) => 'C',
            Some(Container::Cuckoo(_)) => 'K',
            Some(_) => {
                println!("Ошибка: {} не является хеш-таблицей", name);
                '\0'
            }
        }
    }

    fn print_any(&self, name: &str) {
        match self.containers.get(name) {
            None => println!("Ошибка: Контейнер {} не найден", name),
            Some(Container::Set(value)) => value.print(),
            Some(Container::Tree(value)) => value.print_wide(),
            Some(Container::Chain(value)) => value.print(),
            Some(Container::Cuckoo(value)) => value.print(),
            Some(Container::Lfu(value)) => value.print(),
        }
    }
}

fn parse_key(value: &str) -> Option<i32> {
    match value.parse::<i32>() {
        Ok(number) => Some(number),
        Err(_) => {
            println!("Ошибка: Ключ дерева должен быть целым числом");
            None
        }
    }
}

fn parse_capacity(value: &str) -> Option<usize> {
    match value.parse::<i64>() {
        Ok(number) if number >= 0 => Some(number as usize),
        _ => {
            println!("Ошибка: Ёмкость должна быть неотрицательным целым числом");
            None
        }
    }
}

fn load_db(db: &mut Db, file: &str) {
    let content = match fs::read_to_string(file) {
        Ok(value) => value,
        Err(_) => return,
    };

    for line in content.lines() {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() < 2 {
            continue;
        }
        let kind = parts[0];
        let name = parts[1];
        let values = &parts[2..];

        match kind {
            "H" => {
                if !db.ensure(name, 'H') {
                    continue;
                }
                if let Some(Container::Set(set)) = db.containers.get_mut(name) {
                    for value in values {
                        if !set.contains(value) {
                            set.add((*value).to_string());
                        }
                    }
                }
            }
            "T" => {
                if !db.ensure(name, 'T') {
                    continue;
                }
                if let Some(Container::Tree(tree)) = db.containers.get_mut(name) {
                    for value in values {
                        if let Some(key) = value.parse::<i32>().ok() {
                            tree.insert(key);
                        } else {
                            println!("Ошибка: Ключ дерева должен быть целым числом");
                        }
                    }
                }
            }
            "C" | "K" => {
                if values.len() % 2 != 0 {
                    println!("Ошибка: В строке таблицы {} нечётное число значений", name);
                    continue;
                }
                let target_kind = if kind == "C" { 'C' } else { 'K' };
                if !db.ensure(name, target_kind) {
                    continue;
                }
                if target_kind == 'C' {
                    if let Some(Container::Chain(table)) = db.containers.get_mut(name) {
                        for pair in values.chunks_exact(2) {
                            table.put(pair[0].to_string(), pair[1].to_string());
                        }
                    }
                } else if let Some(Container::Cuckoo(table)) = db.containers.get_mut(name) {
                    for pair in values.chunks_exact(2) {
                        table.put(pair[0].to_string(), pair[1].to_string());
                    }
                }
            }
            "U" => {
                if values.len() < 2 || (values.len() - 2) % 4 != 0 {
                    println!("Ошибка: Неверная строка кэша {} в файле", name);
                    continue;
                }
                let capacity = match values[0].parse::<usize>() {
                    Ok(value) => value,
                    Err(_) => {
                        println!("Ошибка: Неверная строка кэша {} в файле", name);
                        continue;
                    }
                };
                let clock = match values[1].parse::<u64>() {
                    Ok(value) => value,
                    Err(_) => {
                        println!("Ошибка: Неверная строка кэша {} в файле", name);
                        continue;
                    }
                };
                let mut parsed = Vec::new();
                let mut valid = true;
                for entry in values[2..].chunks_exact(4) {
                    let frequency = entry[2].parse::<u64>();
                    let stamp = entry[3].parse::<u64>();
                    match (frequency, stamp) {
                        (Ok(frequency), Ok(stamp)) => parsed.push((entry[0], entry[1], frequency, stamp)),
                        _ => valid = false,
                    }
                }
                if !valid || !db.ensure(name, 'U') {
                    println!("Ошибка: Неверная строка кэша {} в файле", name);
                    continue;
                }
                if let Some(Container::Lfu(cache)) = db.containers.get_mut(name) {
                    cache.capacity = capacity;
                    cache.clock = clock;
                    for (key, value, frequency, stamp) in parsed {
                        cache.put_with_metadata(
                            key.to_string(), value.to_string(), frequency, stamp,
                        );
                    }
                }
            }
            _ => println!("Ошибка: Неизвестный тип структуры в файле: {}", kind),
        }
    }
}

fn save_db(db: &Db, file: &str) {
    let mut output = String::new();
    for (name, container) in &db.containers {
        match container {
            Container::Set(set) => {
                output.push_str(&format!("H {}", name));
                for value in set.values_in_bucket_order() {
                    output.push_str(&format!(" {}", value));
                }
                output.push('\n');
            }
            Container::Tree(tree) => {
                output.push_str(&format!("T {}", name));
                for key in tree.breadth_first_keys() {
                    output.push_str(&format!(" {}", key));
                }
                output.push('\n');
            }
            Container::Chain(table) => {
                output.push_str(&format!("C {}", name));
                for (key, value) in table.entries() {
                    output.push_str(&format!(" {} {}", key, value));
                }
                output.push('\n');
            }
            Container::Cuckoo(table) => {
                output.push_str(&format!("K {}", name));
                for (key, value) in table.entries() {
                    output.push_str(&format!(" {} {}", key, value));
                }
                output.push('\n');
            }
            Container::Lfu(cache) => {
                output.push_str(&format!("U {} {} {}", name, cache.capacity, cache.clock));
                for (key, value, frequency, stamp) in cache.entries_with_metadata() {
                    output.push_str(&format!(" {} {} {} {}", key, value, frequency, stamp));
                }
                output.push('\n');
            }
        }
    }

    if let Err(error) = fs::write(file, output) {
        println!("Ошибка: Не удалось записать файл {}: {}", file, error);
    }
}

fn expected_args(command: &str) -> Option<(usize, bool)> {
    let info = match command {
        "PRINT" => (2, false),
        "SETCREATE" => (2, true),
        "SETADD" => (3, true),
        "SETDEL" => (3, true),
        "SET_AT" => (3, false),
        "TCREATE" => (2, true),
        "TINSERT" => (3, true),
        "TSNAKE" => (2, false),
        "HCREATE" => (3, true),
        "HSET" => (4, true),
        "HGET" => (3, false),
        "HDEL" => (3, true),
        "HSTATS" => (1, false),
        "GENEALOGY" => (0, false),
        "LFUCREATE" => (3, true),
        "LFUSET" => (4, true),
        "LFUGET" => (3, true),
        "ASTEROIDS" => (0, false),
        "TURTLES" => (0, false),
        "GISTOGRAM" => (0, false),
        _ => return None,
    };
    Some(info)
}

fn run(db: &mut Db, command: &str, tokens: &[String]) {
    match command {
        "PRINT" => db.print_any(&tokens[1]),

        "SETCREATE" => {
            db.ensure(&tokens[1], 'H');
        }
        "SETADD" => {
            if db.ensure(&tokens[1], 'H') {
                if let Some(set) = db.get_set_mut(&tokens[1]) {
                    set.add(tokens[2].clone());
                }
            }
        }
        "SETDEL" => {
            if let Some(set) = db.get_set_mut(&tokens[1]) {
                set.remove(&tokens[2]);
            }
        }
        "SET_AT" => {
            if let Some(set) = db.get_set_mut(&tokens[1]) {
                println!("{}", if set.contains(&tokens[2]) { "TRUE" } else { "FALSE" });
            }
        }

        "TCREATE" => {
            db.ensure(&tokens[1], 'T');
        }
        "TINSERT" => {
            if db.ensure(&tokens[1], 'T') {
                if let Some(key) = parse_key(&tokens[2]) {
                    if let Some(tree) = db.get_tree_mut(&tokens[1]) {
                        tree.insert(key);
                    }
                }
            }
        }
        "TSNAKE" => {
            if let Some(tree) = db.get_tree_mut(&tokens[1]) {
                tree.print_snake();
            }
        }

        "HCREATE" => {
            match tokens[2].to_ascii_uppercase().as_str() {
                "CHAIN" => {
                    db.ensure(&tokens[1], 'C');
                }
                "CUCKOO" => {
                    db.ensure(&tokens[1], 'K');
                }
                _ => println!("Ошибка: Тип таблицы должен быть CHAIN или CUCKOO"),
            }
        }
        "HSET" => {
            if !db.containers.contains_key(&tokens[1]) {
                if db.ensure(&tokens[1], 'C') {
                    if let Some(Container::Chain(table)) = db.containers.get_mut(&tokens[1]) {
                        table.put(tokens[2].clone(), tokens[3].clone());
                    }
                }
            } else {
                match db.hash_kind(&tokens[1]) {
                    'C' => {
                        if let Some(Container::Chain(table)) = db.containers.get_mut(&tokens[1]) {
                            table.put(tokens[2].clone(), tokens[3].clone());
                        }
                    }
                    'K' => {
                        if let Some(Container::Cuckoo(table)) = db.containers.get_mut(&tokens[1]) {
                            table.put(tokens[2].clone(), tokens[3].clone());
                        }
                    }
                    _ => {}
                }
            }
        }
        "HGET" => {
            let kind = db.hash_kind(&tokens[1]);
            let value = match (kind, db.containers.get(&tokens[1])) {
                ('C', Some(Container::Chain(table))) => table.get(&tokens[2]),
                ('K', Some(Container::Cuckoo(table))) => table.get(&tokens[2]),
                _ => None,
            };
            if kind != '\0' {
                match value {
                    Some(value) => println!("{}", value),
                    None => println!("Ошибка: Ключ не найден"),
                }
            }
        }
        "HDEL" => {
            let kind = db.hash_kind(&tokens[1]);
            let removed = match (kind, db.containers.get_mut(&tokens[1])) {
                ('C', Some(Container::Chain(table))) => table.remove(&tokens[2]),
                ('K', Some(Container::Cuckoo(table))) => table.remove(&tokens[2]),
                _ => false,
            };
            if kind != '\0' && !removed {
                println!("Ошибка: Ключ не найден");
            }
        }

        "HSTATS" => {
            if !hashtable::hash_analysis() {
                std::process::exit(1);
            }
        }

        "GENEALOGY" => {
            genealogy(tokens);
        }

        "LFUCREATE" => {
            let Some(capacity) = parse_capacity(&tokens[2]) else {
                return;
            };
            if matches!(db.containers.get(&tokens[1]), Some(Container::Lfu(_))) {
                println!("Ошибка: Кэш {} уже существует", tokens[1]);
                return;
            }
            if db.ensure(&tokens[1], 'U') {
                if let Some(Container::Lfu(cache)) = db.containers.get_mut(&tokens[1]) {
                    cache.capacity = capacity;
                }
            }
        }
        "LFUSET" => {
            if let Some(cache) = db.get_lfu_mut(&tokens[1]) {
                cache.set(tokens[2].clone(), tokens[3].clone());
            }
        }
        "LFUGET" => {
            if let Some(cache) = db.get_lfu_mut(&tokens[1]) {
                match cache.get(&tokens[2]) {
                    Some(value) => println!("{}", value),
                    None => println!("-1"),
                }
            }
        }

        "ASTEROIDS" => {
            let input = tokens[1..].join(" ");
            match asteroids::parse(&input) {
                Ok(parsed) => {
                    let result = asteroids::resolve(&parsed);
                    asteroids::print_result(&result);
                }
                Err(error) => println!("{}", error),
            }
        }
        "TURTLES" => {
            turtles(tokens);
        }
        "GISTOGRAM" => {
            let mut array = Arr::new();
            for token in &tokens[1..] {
                for byte in token.bytes() {
                    array.insert_byte(byte);
                }
            }
            array.histogram();
        }
        _ => {}
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut file = String::new();
    let mut query = String::new();
    let mut index = 1usize;
    while index < args.len() {
        match args[index].as_str() {
            "--file" if index + 1 < args.len() => {
                file = args[index + 1].clone();
                index += 2;
            }
            "--query" if index + 1 < args.len() => {
                query = args[index + 1].clone();
                index += 2;
            }
            _ => index += 1,
        }
    }

    if file.is_empty() || query.is_empty() {
        println!("Использование: ./lab2_rust --file file.data --query 'КОМАНДА имя [аргументы]'");
        std::process::exit(1);
    }

    let mut tokens: Vec<String> = query.split_whitespace().map(str::to_string).collect();
    if tokens.is_empty() {
        println!("Ошибка: Пустой запрос");
        std::process::exit(1);
    }
    tokens[0] = tokens[0].to_ascii_uppercase();
    let command = tokens[0].clone();

    let Some((required, changes)) = expected_args(&command) else {
        println!("Ошибка: Неизвестная команда {}", command);
        std::process::exit(1);
    };
    if required != 0 && tokens.len() != required {
        println!("Ошибка: Неверное число аргументов для {}", command);
        std::process::exit(1);
    }

    let mut db = Db::default();
    load_db(&mut db, &file);
    run(&mut db, &command, &tokens);

    if changes {
        save_db(&db, &file);
    }
}
