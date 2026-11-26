use crate::ulist::UList;

pub fn hash1(key: &str, m: usize) -> usize {
    if m == 0 {
        return 0;
    }
    let m = m as u64;
    let mut h = 0u64;
    for byte in key.bytes() {
        h = (h.wrapping_mul(31).wrapping_add(u64::from(byte))) % m;
    }
    h as usize
}

pub fn hash2(key: &str, m: usize) -> usize {
    if m == 0 {
        return 0;
    }
    let m = m as u64;
    let mut h = 7u64;
    for byte in key.bytes() {
        h = (h.wrapping_mul(131).wrapping_add(u64::from(byte))) % m;
    }
    h as usize
}

pub struct ChainTable {
    buckets: Vec<UList>,
    cap: usize,
    sz: usize,
}

impl Default for ChainTable {
    fn default() -> Self {
        Self::new(101)
    }
}

impl ChainTable {
    pub fn new(c: usize) -> Self {
        let cap = if c > 0 { c } else { 101 };
        let buckets = (0..cap).map(|_| UList::new()).collect();
        Self { buckets, cap, sz: 0 }
    }

    pub fn put(&mut self, key: String, value: String) {
        let i = hash1(&key, self.cap);
        if self.buckets[i].update_value(&key, &value) {
            return;
        }
        self.buckets[i].insert_head_kv(key, value);
        self.sz += 1;
    }

    pub fn get(&self, key: &str) -> Option<String> {
        let i = hash1(key, self.cap);
        self.buckets[i].get_value(key)
    }

    pub fn remove(&mut self, key: &str) -> bool {
        let i = hash1(key, self.cap);
        if !self.buckets[i].remove_key(key) {
            return false;
        }
        self.sz -= 1;
        true
    }

    pub fn print(&self) {
        if self.sz == 0 {
            println!("Хеш-таблица пуста");
            return;
        }
        println!("Хеш-таблица (цепочки):");
        for (i, bucket) in self.buckets.iter().enumerate() {
            if bucket.size() == 0 {
                continue;
            }
            print!("[{}] ", i);
            bucket.print_pairs();
        }
    }

    pub fn size(&self) -> usize {
        self.sz
    }

    pub fn capacity(&self) -> usize {
        self.cap
    }

    pub fn get_all(&self) -> Vec<(String, String)> {
        let mut res = Vec::new();
        for bucket in &self.buckets {
            res.extend(bucket.get_pairs());
        }
        res
    }
}

type Slot = Option<(String, String)>;

pub struct CuckooTable {
    t1: Vec<Slot>,
    t2: Vec<Slot>,
    sz: usize,
    grows: usize,
    kicks: u64,
}

impl Default for CuckooTable {
    fn default() -> Self {
        Self::new(11)
    }
}

impl CuckooTable {
    pub fn new(c: usize) -> Self {
        let cap = if c > 0 { c } else { 11 };
        Self {
            t1: vec![None; cap],
            t2: vec![None; cap],
            sz: 0,
            grows: 0,
            kicks: 0,
        }
    }

    pub fn capacity(&self) -> usize {
        self.t1.len()
    }

    pub fn size(&self) -> usize {
        self.sz
    }

    pub fn grows(&self) -> usize {
        self.grows
    }

    pub fn kicks(&self) -> u64 {
        self.kicks
    }

    fn insert_raw(&mut self, mut key: String, mut value: String) -> Result<(), (String, String)> {
        for _ in 0..32 {
            let i = hash1(&key, self.capacity());
            match self.t1[i].replace((key, value)) {
                None => {
                    self.sz += 1;
                    return Ok(());
                }
                Some((k, v)) => {
                    key = k;
                    value = v;
                    self.kicks += 1;
                }
            }

            let j = hash2(&key, self.capacity());
            match self.t2[j].replace((key, value)) {
                None => {
                    self.sz += 1;
                    return Ok(());
                }
                Some((k, v)) => {
                    key = k;
                    value = v;
                    self.kicks += 1;
                }
            }
        }
        Err((key, value))
    }

    fn grow_and_rehash(&mut self, pending: (String, String)) {
        let mut all: Vec<(String, String)> = Vec::with_capacity(self.sz + 1);
        for slot in self.t1.iter_mut().chain(self.t2.iter_mut()) {
            if let Some(entry) = slot.take() {
                all.push(entry);
            }
        }
        all.push(pending);

        loop {
            let new_cap = self.capacity().saturating_mul(2).saturating_add(1);
            self.t1 = vec![None; new_cap];
            self.t2 = vec![None; new_cap];
            self.sz = 0;
            self.grows += 1;

            let mut failed: Vec<(String, String)> = Vec::new();
            for (key, value) in all.drain(..) {
                if let Err(entry) = self.insert_raw(key, value) {
                    failed.push(entry);
                }
            }
            if failed.is_empty() {
                break;
            }

            all = self
                .t1
                .iter_mut()
                .chain(self.t2.iter_mut())
                .filter_map(|slot| slot.take())
                .collect();
            all.extend(failed);
        }
    }

    pub fn put(&mut self, key: String, value: String) {
        if key.is_empty() {
            return;
        }
        let i = hash1(&key, self.capacity());
        if let Some((stored_key, stored_value)) = self.t1[i].as_mut() {
            if stored_key.as_str() == key.as_str() {
                *stored_value = value;
                return;
            }
        }
        let j = hash2(&key, self.capacity());
        if let Some((stored_key, stored_value)) = self.t2[j].as_mut() {
            if stored_key.as_str() == key.as_str() {
                *stored_value = value;
                return;
            }
        }
        if let Err(pending) = self.insert_raw(key, value) {
            self.grow_and_rehash(pending);
        }
    }

    pub fn get(&self, key: &str) -> Option<String> {
        if key.is_empty() {
            return None;
        }
        let i = hash1(key, self.capacity());
        if let Some((stored_key, value)) = &self.t1[i] {
            if stored_key.as_str() == key {
                return Some(value.clone());
            }
        }
        let j = hash2(key, self.capacity());
        if let Some((stored_key, value)) = &self.t2[j] {
            if stored_key.as_str() == key {
                return Some(value.clone());
            }
        }
        None
    }

    pub fn remove(&mut self, key: &str) -> bool {
        if key.is_empty() {
            return false;
        }
        let i = hash1(key, self.capacity());
        if self.t1[i].as_ref().is_some_and(|(k, _)| k.as_str() == key) {
            self.t1[i] = None;
            self.sz -= 1;
            return true;
        }
        let j = hash2(key, self.capacity());
        if self.t2[j].as_ref().is_some_and(|(k, _)| k.as_str() == key) {
            self.t2[j] = None;
            self.sz -= 1;
            return true;
        }
        false
    }

    pub fn print(&self) {
        if self.sz == 0 {
            println!("Хеш-таблица пуста");
            return;
        }
        println!("Хеш-таблица (кукушка):");
        for (i, slot) in self.t1.iter().enumerate() {
            if let Some((k, v)) = slot {
                println!("T1[{}] {}={}", i, k, v);
            }
        }
        for (i, slot) in self.t2.iter().enumerate() {
            if let Some((k, v)) = slot {
                println!("T2[{}] {}={}", i, k, v);
            }
        }
    }

    pub fn get_all(&self) -> Vec<(String, String)> {
        self.t1.iter().chain(self.t2.iter()).flatten().cloned().collect()
    }
}

#[cfg(test)]
#[path = "../tests/unit/hashtable.rs"]
mod tests;