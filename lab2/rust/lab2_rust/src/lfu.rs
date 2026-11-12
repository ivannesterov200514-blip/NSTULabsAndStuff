use crate::hashtable::ChainTable;

#[derive(Debug, Clone)]
pub struct Lfu {
    pub capacity: usize,
    pub clock: u64,
    pub data: ChainTable,
}

impl Default for Lfu {
    fn default() -> Self {
        Self::new(2)
    }
}

impl Lfu {
    pub fn new(capacity: usize) -> Self {
        Self {
            capacity,
            clock: 0,
            data: ChainTable::new(101),
        }
    }

    fn parse_entry(entry: &str) -> Option<(String, u64, u64)> {
        let mut parts = entry.split_whitespace();
        let value = parts.next()?.to_string();
        let frequency = parts.next()?.parse::<u64>().ok()?;
        let stamp = parts.next()?.parse::<u64>().ok()?;
        if parts.next().is_some() {
            return None;
        }
        Some((value, frequency, stamp))
    }

    pub fn put_with_metadata(
        &mut self,
        key: String,
        value: String,
        frequency: u64,
        stamp: u64,
    ) {
        self.data.put(
            key,
            format!("{} {} {}", value, frequency, stamp),
        );
    }

    fn read(&self, key: &str) -> Option<(String, u64, u64)> {
        Self::parse_entry(self.data.get(key)?)
    }

    pub fn get(&mut self, key: &str) -> Option<String> {
        let (value, frequency, _) = self.read(key)?;
        self.clock += 1;
        self.put_with_metadata(key.to_string(), value.clone(), frequency + 1, self.clock);
        Some(value)
    }

    fn evict_one(&mut self) {
        let mut worst: Option<(String, u64, u64)> = None;
        for (key, encoded) in self.data.entries() {
            let Some((_, frequency, stamp)) = Self::parse_entry(encoded) else {
                continue;
            };
            let replace = match &worst {
                None => true,
                Some((_, worst_frequency, worst_stamp)) => {
                    frequency < *worst_frequency
                        || (frequency == *worst_frequency && stamp < *worst_stamp)
                }
            };
            if replace {
                worst = Some((key.clone(), frequency, stamp));
            }
        }
        if let Some((key, _, _)) = worst {
            self.data.remove(&key);
        }
    }

    pub fn set(&mut self, key: String, value: String) {
        if self.capacity == 0 {
            return;
        }
        self.clock += 1;
        if let Some((_, frequency, _)) = self.read(&key) {
            self.put_with_metadata(key, value, frequency + 1, self.clock);
            return;
        }
        if self.data.size >= self.capacity {
            self.evict_one();
        }
        self.put_with_metadata(key, value, 1, self.clock);
    }

    pub fn print(&self) {
        if self.data.size == 0 {
            println!("Кэш пуст");
            return;
        }
        println!("Кэш (емкость {}):", self.capacity);
        for (key, encoded) in self.data.entries() {
            if let Some((value, frequency, _)) = Self::parse_entry(encoded) {
                println!("{}={} (использований: {})", key, value, frequency);
            }
        }
    }

    pub fn entries_with_metadata(&self) -> Vec<(String, String, u64, u64)> {
        self.data
            .entries()
            .filter_map(|(key, encoded)| {
                Self::parse_entry(encoded).map(|(value, frequency, stamp)| {
                    (key.clone(), value, frequency, stamp)
                })
            })
            .collect()
    }
}
