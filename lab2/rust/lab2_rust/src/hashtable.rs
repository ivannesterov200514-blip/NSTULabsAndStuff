use std::collections::{HashMap, VecDeque};

pub fn hash1(key: &str, capacity: usize) -> usize {
    let mut hash = 0u64;
    for byte in key.bytes() {
        hash = (hash.wrapping_mul(31).wrapping_add(u64::from(byte))) % capacity as u64;
    }
    hash as usize
}

pub fn hash2(key: &str, capacity: usize) -> usize {
    let mut hash = 7u64;
    for byte in key.bytes() {
        hash = (hash.wrapping_mul(131).wrapping_add(u64::from(byte))) % capacity as u64;
    }
    hash as usize
}

#[derive(Debug, Clone)]
pub struct ChainTable {
    buckets: Vec<Vec<(String, String)>>,
    pub size: usize,
}

impl Default for ChainTable {
    fn default() -> Self {
        Self::new(101)
    }
}

impl ChainTable {
    pub fn new(capacity: usize) -> Self {
        let capacity = capacity.max(1);
        Self {
            buckets: vec![Vec::new(); capacity],
            size: 0,
        }
    }

    pub fn capacity(&self) -> usize {
        self.buckets.len()
    }

    pub fn put(&mut self, key: String, value: String) {
        let index = hash1(&key, self.capacity());
        if let Some(position) = self.buckets[index]
            .iter()
            .position(|(stored_key, _)| stored_key.as_str() == key.as_str())
        {
            self.buckets[index][position].1 = value;
            return;
        }
        self.buckets[index].insert(0, (key, value));
        self.size += 1;
    }

    pub fn get(&self, key: &str) -> Option<&str> {
        self.get_counted(key, &mut 0)
    }

    fn get_counted(&self, key: &str, comparisons: &mut u64) -> Option<&str> {
        let index = hash1(key, self.capacity());
        for (stored_key, value) in &self.buckets[index] {
            *comparisons += 1;
            if stored_key.as_str() == key {
                return Some(value.as_str());
            }
        }
        None
    }

    pub fn remove(&mut self, key: &str) -> bool {
        let index = hash1(key, self.capacity());
        if let Some(position) = self.buckets[index]
            .iter()
            .position(|(stored_key, _)| stored_key.as_str() == key)
        {
            self.buckets[index].remove(position);
            self.size -= 1;
            true
        } else {
            false
        }
    }

    pub fn entries(&self) -> impl Iterator<Item = (&String, &String)> {
        self.buckets
            .iter()
            .flat_map(|bucket| bucket.iter().map(|(key, value)| (key, value)))
    }

    pub fn print(&self) {
        if self.size == 0 {
            println!("Хеш-таблица пуста");
            return;
        }
        println!("Хеш-таблица (цепочки):");
        for (index, bucket) in self.buckets.iter().enumerate() {
            if bucket.is_empty() {
                continue;
            }
            print!("[{}] ", index);
            for (key, value) in bucket {
                print!("{}={} -> ", key, value);
            }
            println!("nullptr");
        }
    }
}

#[derive(Debug, Clone)]
pub struct CuckooTable {
    t1: Vec<Option<(String, String)>>,
    t2: Vec<Option<(String, String)>>,
    pub size: usize,
    pub grows: usize,
    pub kicks: u64,
}

impl Default for CuckooTable {
    fn default() -> Self {
        Self::new(11)
    }
}

impl CuckooTable {
    pub fn new(capacity: usize) -> Self {
        let capacity = capacity.max(3);
        Self {
            t1: vec![None; capacity],
            t2: vec![None; capacity],
            size: 0,
            grows: 0,
            kicks: 0,
        }
    }

    pub fn capacity(&self) -> usize {
        self.t1.len()
    }

    fn insert_raw(&mut self, mut key: String, mut value: String) -> Result<(), (String, String)> {
        for _ in 0..32 {
            let index1 = hash1(&key, self.capacity());
            let previous = self.t1[index1].replace((key, value));
            match previous {
                None => {
                    self.size += 1;
                    return Ok(());
                }
                Some((old_key, old_value)) => {
                    key = old_key;
                    value = old_value;
                    self.kicks += 1;
                }
            }

            let index2 = hash2(&key, self.capacity());
            let previous = self.t2[index2].replace((key, value));
            match previous {
                None => {
                    self.size += 1;
                    return Ok(());
                }
                Some((old_key, old_value)) => {
                    key = old_key;
                    value = old_value;
                    self.kicks += 1;
                }
            }
        }
        Err((key, value))
    }

    fn grow_and_rehash(&mut self, pending: (String, String)) {
        let mut all = Vec::<(String, String)>::with_capacity(self.size + 1);
        for slot in self.t1.iter_mut().chain(self.t2.iter_mut()) {
            if let Some(entry) = slot.take() {
                all.push(entry);
            }
        }
        all.push(pending);

        loop {
            let new_capacity = self.capacity().saturating_mul(2).saturating_add(1).max(3);
            self.t1 = vec![None; new_capacity];
            self.t2 = vec![None; new_capacity];
            self.size = 0;
            self.grows += 1;

            let mut pending_again = Vec::new();
            for (key, value) in all.drain(..) {
                if let Err(entry) = self.insert_raw(key, value) {
                    pending_again.push(entry);
                }
            }
            if pending_again.is_empty() {
                break;
            }

            all = self
                .t1
                .iter_mut()
                .chain(self.t2.iter_mut())
                .filter_map(|slot| slot.take())
                .collect();
            all.extend(pending_again);
        }
    }

    pub fn put(&mut self, key: String, value: String) {
        if key.is_empty() {
            return;
        }
        let index1 = hash1(&key, self.capacity());
        if let Some((stored_key, stored_value)) = self.t1[index1].as_mut() {
            if stored_key.as_str() == key.as_str() {
                *stored_value = value;
                return;
            }
        }
        let index2 = hash2(&key, self.capacity());
        if let Some((stored_key, stored_value)) = self.t2[index2].as_mut() {
            if stored_key.as_str() == key.as_str() {
                *stored_value = value;
                return;
            }
        }
        if let Err(pending) = self.insert_raw(key, value) {
            self.grow_and_rehash(pending);
        }
    }

    pub fn get(&self, key: &str) -> Option<&str> {
        self.get_counted(key, &mut 0)
    }

    fn get_counted(&self, key: &str, comparisons: &mut u64) -> Option<&str> {
        if key.is_empty() {
            return None;
        }
        let index1 = hash1(key, self.capacity());
        *comparisons += 1;
        if let Some((stored_key, value)) = &self.t1[index1] {
            if stored_key.as_str() == key {
                return Some(value);
            }
        }
        let index2 = hash2(key, self.capacity());
        *comparisons += 1;
        if let Some((stored_key, value)) = &self.t2[index2] {
            if stored_key.as_str() == key {
                return Some(value);
            }
        }
        None
    }

    pub fn remove(&mut self, key: &str) -> bool {
        if key.is_empty() {
            return false;
        }
        let index1 = hash1(key, self.capacity());
        if self.t1[index1].as_ref().map_or(false, |(stored_key, _)| stored_key.as_str() == key) {
            self.t1[index1] = None;
            self.size -= 1;
            return true;
        }
        let index2 = hash2(key, self.capacity());
        if self.t2[index2].as_ref().map_or(false, |(stored_key, _)| stored_key.as_str() == key) {
            self.t2[index2] = None;
            self.size -= 1;
            return true;
        }
        false
    }

    pub fn entries(&self) -> Vec<(String, String)> {
        let mut result = Vec::with_capacity(self.size);
        for index in 0..self.capacity() {
            if let Some(entry) = self.t1[index].clone() {
                result.push(entry);
            }
            if let Some(entry) = self.t2[index].clone() {
                result.push(entry);
            }
        }
        result
    }

    pub fn print(&self) {
        if self.size == 0 {
            println!("Хеш-таблица пуста");
            return;
        }
        println!("Хеш-таблица (кукушка):");
        for (index, slot) in self.t1.iter().enumerate() {
            if let Some((key, value)) = slot {
                println!("T1[{}] {}={}", index, key, value);
            }
        }
        for (index, slot) in self.t2.iter().enumerate() {
            if let Some((key, value)) = slot {
                println!("T2[{}] {}={}", index, key, value);
            }
        }
    }
}

pub fn genealogy(tokens: &[String]) -> bool {
    let n = tokens
        .get(1)
        .and_then(|s| s.parse::<usize>().ok())
        .unwrap_or(0);
    if n < 2 {
        println!("Ошибка: N должно быть целым числом не меньше 2");
        return false;
    }
    if tokens.len() != 2 + 2 * (n - 1) {
        println!("Ошибка: Неверное число пар «потомок родитель» (для N элементов нужно N-1 пар)");
        return false;
    }

    let mut parent = HashMap::<String, String>::new();
    let mut children = HashMap::<String, Vec<String>>::new();
    let mut names = Vec::<String>::new();
    let mut seen = std::collections::HashSet::<String>::new();

    for i in 0..n - 1 {
        let child = tokens[2 + 2 * i].clone();
        let father = tokens[3 + 2 * i].clone();
        if parent.contains_key(&child) {
            println!("Ошибка: У элемента {} больше одного родителя", child);
            return false;
        }
        parent.insert(child.clone(), father.clone());
        children.entry(father.clone()).or_default().push(child.clone());
        if seen.insert(child.clone()) {
            names.push(child);
        }
        if seen.insert(father.clone()) {
            names.push(father);
        }
    }

    let roots: Vec<String> = names
        .iter()
        .filter(|name| !parent.contains_key(*name))
        .cloned()
        .collect();
    if roots.is_empty() {
        println!("Ошибка: Родоначальник не найден (в данных есть цикл)");
        return false;
    }
    if roots.len() > 1 {
        println!("Ошибка: В данных больше одного родоначальника");
        return false;
    }
    let root = roots[0].clone();

    let mut order = Vec::<String>::new();
    let mut queue = VecDeque::<String>::new();
    queue.push_back(root.clone());
    while let Some(name) = queue.pop_front() {
        order.push(name.clone());
        if let Some(kids) = children.get(&name) {
            for child in kids {
                queue.push_back(child.clone());
            }
        }
    }
    if order.len() != names.len() {
        println!("Ошибка: В данных есть цикл, не все элементы связаны с родоначальником");
        return false;
    }

    let mut counts = HashMap::<String, usize>::new();
    for name in &order {
        counts.insert(name.clone(), 0);
    }
    for name in order.iter().rev().take(order.len().saturating_sub(1)) {
        if let Some(father) = parent.get(name) {
            let descendants = counts.get(name).copied().unwrap_or(0) + 1;
            *counts.entry(father.clone()).or_insert(0) += descendants;
        }
    }

    let mut sorted = names.clone();
    sorted.sort();
    for name in sorted {
        println!("{} {}", name, counts.get(&name).copied().unwrap_or(0));
    }

    println!("\nДрево:");
    let mut stack = vec![(root, 0usize)];
    while let Some((name, depth)) = stack.pop() {
        println!("{}{}", " ".repeat(depth * 2), name);
        if let Some(kids) = children.get(&name) {
            let mut sorted_children = kids.clone();
            sorted_children.sort();
            for child in sorted_children.into_iter().rev() {
                stack.push((child, depth + 1));
            }
        }
    }
    true
}

// Эмпирический анализ хеш-таблиц
pub fn hash_analysis() -> bool {
    use std::time::Instant;
    for n in [100usize, 1000, 10000] {
        let keys: Vec<String> = (0..n).map(|i| format!("key{}", i)).collect();
        let mut chain = ChainTable::new(102);
        let mut cuckoo = CuckooTable::new(51);
        println!("\nN = {}", n);
        for method in 0..2 {
            let mut comparisons = 0u64;
            let start = Instant::now();
            for key in &keys {
                if method == 0 {
                    chain.put(key.clone(), key.clone());
                } else {
                    cuckoo.put(key.clone(), key.clone());
                }
            }
            let insert = start.elapsed().as_secs_f64() * 1_000_000.0;
            let start = Instant::now();
            for key in &keys {
                let value = if method == 0 {
                    chain.get_counted(key, &mut comparisons)
                } else {
                    cuckoo.get_counted(key, &mut comparisons)
                };
                if value != Some(key.as_str()) {
                    return false;
                }
            }
            let search = start.elapsed().as_secs_f64() * 1_000_000.0;
            let capacity = if method == 0 { chain.capacity() } else { 2 * cuckoo.capacity() };
            let mut occupied = 0;
            let mut max_length = 0;
            if method == 0 {
                for bucket in &chain.buckets {
                    if !bucket.is_empty() { occupied += 1; }
                    max_length = max_length.max(bucket.len());
                }
            }
            let start = Instant::now();
            for key in &keys {
                let removed = if method == 0 { chain.remove(key) } else { cuckoo.remove(key) };
                if !removed { return false; }
            }
            let removal = start.elapsed().as_secs_f64() * 1_000_000.0;
            if (if method == 0 { chain.size } else { cuckoo.size }) != 0 { return false; }
            println!("{}", if method == 0 { "CHAIN" } else { "CUCKOO" });
            println!("Insert (us): {:.4}; avg: {:.4}", insert, insert / n as f64);
            println!("Search (us): {:.4}; avg: {:.4}", search, search / n as f64);
            println!("Remove (us): {:.4}; avg: {:.4}", removal, removal / n as f64);
            println!("Capacity: {}", capacity);
            println!("Comparisons: {}; avg: {:.4}", comparisons, comparisons as f64 / n as f64);
            if method == 0 {
                println!("Collisions: {}", n - occupied);
                println!("Max chain: {}", max_length);
            } else {
                println!("Kicks: {}", cuckoo.kicks);
                println!("Grows: {}", cuckoo.grows);
            }
        }
    }
    true
}
