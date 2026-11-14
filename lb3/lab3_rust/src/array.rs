/// Динамический массив строк (аналог класса `Arr` из C++).
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

    /// Проверяет индекс. `allow_end` разрешает индекс, равный длине (для вставки).
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
mod tests {
    use super::*;

    fn s(v: &str) -> String {
        v.to_string()
    }

    #[test]
    fn insert_and_search() {
        let mut a = Arr::default();
        a.insert_tail(s("a"));
        a.insert_tail(s("b"));
        a.insert_tail(s("c"));
        assert_eq!(a.size(), 3);
        assert_eq!(a.search(1), Some(s("b")));
        assert_eq!(a.search(3), None);
        assert_eq!(a.search(-1), None);
    }

    #[test]
    fn insert_by_index() {
        let mut a = Arr::new(1);
        assert!(a.insert_idx(s("x"), 0));
        assert!(a.insert_idx(s("z"), 1));
        assert!(a.insert_idx(s("y"), 1));
        assert!(!a.insert_idx(s("bad"), 5));
        assert!(!a.insert_idx(s("bad"), -1));
        assert_eq!(a.items(), vec![s("x"), s("y"), s("z")]);
    }

    #[test]
    fn delete_and_change() {
        let mut a = Arr::new(0);
        for v in ["1", "2", "3", "4"] {
            a.insert_tail(s(v));
        }
        assert!(a.delete_idx(1));
        assert!(!a.delete_idx(3));
        assert!(a.change_idx(s("9"), 0));
        assert!(!a.change_idx(s("9"), 10));
        assert_eq!(a.items(), vec![s("9"), s("3"), s("4")]);
        assert!(a.capacity() >= 3);
    }

    #[test]
    fn print_does_not_panic() {
        let mut a = Arr::default();
        a.print();
        a.insert_tail(s("q"));
        a.print();
    }
}
