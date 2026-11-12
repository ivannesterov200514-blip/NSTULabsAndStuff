use crate::ulist::UList;

const DEFAULT_CAPACITY: usize = 4099;

#[derive(Debug, Clone)]
pub struct Set {
    table: Vec<UList>,
    pub size: usize,
}

impl Default for Set {
    fn default() -> Self {
        Self::new()
    }
}

impl Set {
    pub fn new() -> Self {
        Self {
            table: (0..DEFAULT_CAPACITY).map(|_| UList::default()).collect(),
            size: 0,
        }
    }

    fn bucket_index(&self, value: &str) -> usize {
        let mut hash: u64 = 5381;
        for byte in value.bytes() {
            hash = hash.wrapping_mul(33).wrapping_add(u64::from(byte));
        }
        (hash % self.table.len() as u64) as usize
    }

    pub fn add(&mut self, value: String) -> bool {
        let index = self.bucket_index(&value);
        if self.table[index].contains(&value) {
            println!("Ошибка: Элемент уже есть в множестве");
            return false;
        }
        self.table[index].insert_head(value);
        self.size += 1;
        true
    }

    pub fn remove(&mut self, value: &str) -> bool {
        let index = self.bucket_index(value);
        if !self.table[index].remove_key(value) {
            println!("Ошибка: Не найдено");
            return false;
        }
        self.size -= 1;
        true
    }

    pub fn contains(&self, value: &str) -> bool {
        let index = self.bucket_index(value);
        self.table[index].contains(value)
    }

    pub fn print(&self) {
        if self.size == 0 {
            println!("Множество пусто");
            return;
        }
        print!("Множество: ");
        for bucket in &self.table {
            for value in bucket.iter() {
                print!("{} ", value);
            }
        }
        println!();
    }

    pub fn values_in_bucket_order(&self) -> Vec<String> {
        let mut result = Vec::with_capacity(self.size);
        for bucket in &self.table {
            result.extend(bucket.iter().cloned());
        }
        result
    }
}

fn parse_nonnegative_u64(value: &str) -> Option<u64> {
    if value.is_empty() || !value.bytes().all(|b| b.is_ascii_digit()) {
        return None;
    }
    value.parse::<u64>().ok()
}

pub fn turtles(tokens: &[String]) -> bool {
    let n = match tokens.get(1).and_then(|s| parse_nonnegative_u64(s)) {
        Some(value) => value,
        None => {
            println!("Ошибка: N должно быть неотрицательным целым числом");
            return false;
        }
    };

    let pair_tokens = tokens.len().saturating_sub(2);
    if pair_tokens % 2 != 0 || pair_tokens / 2 != n as usize {
        println!(
            "Ошибка: Для {} черепах нужно {} пар чисел (a и b для каждой)",
            n, n
        );
        return false;
    }

    let mut places = Set::new();
    for i in 0..n as usize {
        let a = parse_nonnegative_u64(&tokens[2 + 2 * i]);
        let b = parse_nonnegative_u64(&tokens[3 + 2 * i]);
        let (a, b) = match (a, b) {
            (Some(a), Some(b)) => (a, b),
            _ => {
                println!("Ошибка: Числа a и b должны быть целыми неотрицательными");
                return false;
            }
        };
        if a < n && b == n - 1 - a {
            let place = a.to_string();
            if !places.contains(&place) {
                places.add(place);
            }
        }
    }
    println!("{}", places.size);
    true
}
