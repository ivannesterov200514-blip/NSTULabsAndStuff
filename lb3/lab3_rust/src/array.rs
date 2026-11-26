#[derive(Debug, Clone)]
pub struct Arr {
    data: Vec<String>,
}

impl Default for Arr {
    fn default() -> Self {
        Self::new(2)
    }
}

impl Arr {
    pub fn new(cap: usize) -> Self {
        let cap = if cap > 0 { cap } else { 2 };
        Self { data: Vec::with_capacity(cap) }
    }

    pub fn size(&self) -> usize {
        self.data.len()
    }

    pub fn capacity(&self) -> usize {
        self.data.capacity()
    }

    fn index(&self, idx: isize, allow_end: bool) -> Option<usize> {
        if idx < 0 {
            return None;
        }
        let i = idx as usize;
        let len = self.data.len();
        if i < len || (allow_end && i == len) {
            Some(i)
        } else {
            None
        }
    }

    pub fn search(&self, idx: isize) -> Option<String> {
        match self.index(idx, false) {
            Some(i) => Some(self.data[i].clone()),
            None => {
                println!("Ошибка: Такого индекса нет");
                None
            }
        }
    }

    pub fn insert_tail(&mut self, val: String) {
        self.data.push(val);
    }

    pub fn insert_idx(&mut self, val: String, idx: isize) -> bool {
        match self.index(idx, true) {
            Some(i) => {
                self.data.insert(i, val);
                true
            }
            None => {
                println!("Ошибка: Такого индекса нет");
                false
            }
        }
    }

    pub fn delete_idx(&mut self, idx: isize) -> bool {
        match self.index(idx, false) {
            Some(i) => {
                self.data.remove(i);
                true
            }
            None => {
                println!("Ошибка: Такого индекса нет");
                false
            }
        }
    }

    pub fn change_idx(&mut self, val: String, idx: isize) -> bool {
        match self.index(idx, false) {
            Some(i) => {
                self.data[i] = val;
                true
            }
            None => {
                println!("Ошибка: Такого индекса нет");
                false
            }
        }
    }

    pub fn print(&self) {
        print!("Массив: ");
        for v in &self.data {
            print!("{} ", v);
        }
        println!();
    }

    pub fn items(&self) -> Vec<String> {
        self.data.clone()
    }
}

#[cfg(test)]
#[path = "../tests/unit/array.rs"]
mod tests;