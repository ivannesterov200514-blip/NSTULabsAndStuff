// Простые измерения для ЛР3. Запуск: cargo run --release --bin benchmark -- 2000
// На каждую операцию делаем 3 повтора и выводим среднее время в наносекундах.
use std::time::Instant;
use lab3_rust::array::Arr;
use lab3_rust::ulist::UList;
use lab3_rust::dlist::DList;
use lab3_rust::stack::Stack;
use lab3_rust::queue::Queue;
use lab3_rust::hashtable::{ChainTable, CuckooTable};
use lab3_rust::avltree::AVLTree;

const REPEATS: usize = 3;

fn show_result(name: &str, operation: &str, count: usize, time_ns: u128) {
    let average = time_ns as f64 / REPEATS as f64 / count as f64;
    println!("{},{},{},{}", name, operation, count, average);
}

fn bench_array(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut search_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = Arr::default();
        let start = Instant::now();
        for key in keys { a.insert_tail(key.clone()); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.size();

        // Поиск уже в заполненной структуре.
        let start = Instant::now();
        for i in 0..n { if a.search(i as isize).is_some() { checksum += 1; } }
        search_time += start.elapsed().as_nanos();
    }
    show_result("array", "push_back", n, insert_time);
    show_result("array", "get_by_index", n, search_time);
    checksum
}

fn bench_singly_linked_list(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut search_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = UList::default();
        let start = Instant::now();
        for key in keys { a.insert_head(key.clone()); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.size();

        // Поиск уже в заполненной структуре.
        let start = Instant::now();
        for key in keys { if a.search(key).is_some() { checksum += 1; } }
        search_time += start.elapsed().as_nanos();
    }
    show_result("singly_linked_list", "push_front", n, insert_time);
    show_result("singly_linked_list", "get_by_key", n, search_time);
    checksum
}

fn bench_doubly_linked_list(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut search_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = DList::default();
        let start = Instant::now();
        for key in keys { a.insert_head(key.clone()); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.size();

        // Поиск уже в заполненной структуре.
        let start = Instant::now();
        for key in keys { if a.search(key).is_some() { checksum += 1; } }
        search_time += start.elapsed().as_nanos();
    }
    show_result("doubly_linked_list", "push_front", n, insert_time);
    show_result("doubly_linked_list", "get_by_value", n, search_time);
    checksum
}

fn bench_stack(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = Stack::default();
        let start = Instant::now();
        for key in keys { a.push(key.clone()); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.size();
    }
    show_result("stack", "push", n, insert_time);
    checksum
}

fn bench_queue(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = Queue::default();
        let start = Instant::now();
        for key in keys { a.push(key.clone()); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.size();
    }
    show_result("queue", "push", n, insert_time);
    checksum
}

fn bench_chain_table(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut search_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = ChainTable::default();
        let start = Instant::now();
        for key in keys { a.put(key.clone(), "v".to_string()); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.size();

        // Поиск уже в заполненной структуре.
        let start = Instant::now();
        for key in keys { if a.get(key).is_some() { checksum += 1; } }
        search_time += start.elapsed().as_nanos();
    }
    show_result("chain_table", "put", n, insert_time);
    show_result("chain_table", "get", n, search_time);
    checksum
}

fn bench_cuckoo_table(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut search_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = CuckooTable::default();
        let start = Instant::now();
        for key in keys { a.put(key.clone(), "v".to_string()); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.size();

        // Поиск уже в заполненной структуре.
        let start = Instant::now();
        for key in keys { if a.get(key).is_some() { checksum += 1; } }
        search_time += start.elapsed().as_nanos();
    }
    show_result("cuckoo_table", "put", n, insert_time);
    show_result("cuckoo_table", "get", n, search_time);
    checksum
}

fn bench_avl_tree(keys: &[String]) -> usize {
    let n = keys.len();
    let mut insert_time: u128 = 0;
    let mut search_time: u128 = 0;
    let mut checksum = 0;
    for _ in 0..REPEATS {
        let mut a = AVLTree::default();
        let start = Instant::now();
        for i in 0..n { a.insert(i as i32); }
        insert_time += start.elapsed().as_nanos();
        checksum += a.items().len();

        // Поиск уже в заполненной структуре.
        let start = Instant::now();
        for i in 0..n { if a.search(i as i32) { checksum += 1; } }
        search_time += start.elapsed().as_nanos();
    }
    show_result("avl_tree", "insert", n, insert_time);
    show_result("avl_tree", "search", n, search_time);
    checksum
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut n: usize = 2000;
    if args.len() > 1 {
        n = args[1].parse().unwrap_or(0);
    }
    if n < 1 || n > 100000 {
        eprintln!("Usage: cargo run --release --bin benchmark -- [N: 1..100000]");
        std::process::exit(1);
    }

    let mut keys: Vec<String> = Vec::new();
    for i in 0..n {
        keys.push(format!("key_{}", i));
    }

    println!("structure,operation,elements,mean_ns_per_operation");
    let mut checksum: usize = 0;
    checksum += bench_array(&keys);
    checksum += bench_singly_linked_list(&keys);
    checksum += bench_doubly_linked_list(&keys);
    checksum += bench_stack(&keys);
    checksum += bench_queue(&keys);
    checksum += bench_chain_table(&keys);
    checksum += bench_cuckoo_table(&keys);
    checksum += bench_avl_tree(&keys);
    eprintln!("Checksum: {}", checksum);
}
