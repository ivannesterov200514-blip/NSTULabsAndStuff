/// Односвязный список (аналог `UList` из C++).
/// Узел хранит `data` (ключ) и `value` (используется в цепочках хеш-таблицы).
#[derive(Debug)]
struct Node {
    data: String,
    value: String,
    next: Option<Box<Node>>,
}

#[derive(Debug, Default)]
pub struct UList {
    head: Option<Box<Node>>,
    sz: usize,
}

impl UList {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn size(&self) -> usize {
        self.sz
    }

    fn find(&self, key: &str) -> Option<&Node> {
        let mut cur = self.head.as_deref();
        while let Some(n) = cur {
            if n.data.as_str() == key {
                return Some(n);
            }
            cur = n.next.as_deref();
        }
        None
    }

    fn find_mut(&mut self, key: &str) -> Option<&mut Node> {
        let mut cur = self.head.as_deref_mut();
        while let Some(n) = cur {
            if n.data.as_str() == key {
                return Some(n);
            }
            cur = n.next.as_deref_mut();
        }
        None
    }

    fn position(&self, key: &str) -> Option<usize> {
        let mut cur = self.head.as_deref();
        let mut i = 0;
        while let Some(n) = cur {
            if n.data.as_str() == key {
                return Some(i);
            }
            cur = n.next.as_deref();
            i += 1;
        }
        None
    }

    /// Ссылка (`Option<Box<Node>>`), которая указывает на узел с номером `idx`
    /// (или на конец списка, если `idx == sz`).
    fn link_at(&mut self, idx: usize) -> &mut Option<Box<Node>> {
        let mut link = &mut self.head;
        for _ in 0..idx {
            link = &mut link.as_mut().expect("индекс внутри списка").next;
        }
        link
    }

    fn insert_at(&mut self, idx: usize, data: String, value: String) {
        let link = self.link_at(idx);
        let next = link.take();
        *link = Some(Box::new(Node { data, value, next }));
        self.sz += 1;
    }

    fn remove_at(&mut self, idx: usize) {
        let link = self.link_at(idx);
        if let Some(mut node) = link.take() {
            *link = node.next.take();
            self.sz -= 1;
        }
    }

    pub fn insert_head(&mut self, val: String) {
        self.insert_at(0, val, String::new());
    }

    pub fn insert_head_kv(&mut self, key: String, value: String) {
        self.insert_at(0, key, value);
    }

    pub fn insert_tail(&mut self, val: String) {
        let n = self.sz;
        self.insert_at(n, val, String::new());
    }

    pub fn insert_before(&mut self, key: &str, val: String) {
        match self.position(key) {
            Some(i) => self.insert_at(i, val, String::new()),
            None => println!("Ошибка: Не найдено"),
        }
    }

    pub fn insert_after(&mut self, key: &str, val: String) {
        match self.position(key) {
            Some(i) => self.insert_at(i + 1, val, String::new()),
            None => println!("Ошибка: Не найдено"),
        }
    }

    pub fn delete_head(&mut self) {
        if self.head.is_none() {
            println!("Ошибка: Список пуст");
            return;
        }
        if self.sz == 1 {
            println!("Последний узел списка удален.");
        }
        self.remove_at(0);
    }

    pub fn delete_tail(&mut self) {
        if self.head.is_none() {
            println!("Ошибка: Список пуст");
            return;
        }
        if self.sz == 1 {
            println!("Последний узел списка удален.");
        }
        let last = self.sz - 1;
        self.remove_at(last);
    }

    pub fn delete_before(&mut self, key: &str) {
        if self.sz < 2 {
            println!("Ошибка: Не найдено");
            return;
        }
        match self.position(key) {
            None => println!("Ошибка: Не найдено"),
            Some(0) => println!("Ошибка: Перед элементом ничего нет"),
            Some(i) => self.remove_at(i - 1),
        }
    }

    pub fn delete_after(&mut self, key: &str) {
        match self.position(key) {
            None => println!("Ошибка: Не найдено"),
            Some(i) if i + 1 >= self.sz => println!("Ошибка: После элемента ничего нет"),
            Some(i) => self.remove_at(i + 1),
        }
    }

    pub fn delete_key(&mut self, key: &str) {
        match self.position(key) {
            None => println!("Ошибка: Не найдено"),
            Some(i) => {
                if self.sz == 1 {
                    println!("Последний узел списка удален.");
                }
                self.remove_at(i);
            }
        }
    }

    pub fn search(&self, key: &str) -> Option<String> {
        match self.find(key) {
            Some(n) => {
                println!("{}", n.data);
                Some(n.data.clone())
            }
            None => {
                println!("Ошибка: Не найдено");
                None
            }
        }
    }

    pub fn print_iter(&self) {
        if self.head.is_none() {
            println!("Список пуст");
            return;
        }
        let mut cur = self.head.as_deref();
        while let Some(n) = cur {
            print!("{} -> ", n.data);
            cur = n.next.as_deref();
        }
        println!("nullptr");
    }

    fn print_rec_node(n: Option<&Node>) {
        match n {
            None => println!("nullptr"),
            Some(node) => {
                print!("{} -> ", node.data);
                Self::print_rec_node(node.next.as_deref());
            }
        }
    }

    pub fn print_rec(&self) {
        if self.head.is_none() {
            println!("Список пуст");
            return;
        }
        Self::print_rec_node(self.head.as_deref());
    }

    // ---- методы для хеш-таблицы ----

    pub fn get_value(&self, key: &str) -> Option<String> {
        self.find(key).map(|n| n.value.clone())
    }

    pub fn update_value(&mut self, key: &str, value: &str) -> bool {
        match self.find_mut(key) {
            Some(n) => {
                n.value = value.to_string();
                true
            }
            None => false,
        }
    }

    pub fn remove_key(&mut self, key: &str) -> bool {
        match self.position(key) {
            Some(i) => {
                self.remove_at(i);
                true
            }
            None => false,
        }
    }

    pub fn get_pairs(&self) -> Vec<(String, String)> {
        let mut res = Vec::new();
        let mut cur = self.head.as_deref();
        while let Some(n) = cur {
            res.push((n.data.clone(), n.value.clone()));
            cur = n.next.as_deref();
        }
        res
    }

    pub fn print_pairs(&self) {
        let mut cur = self.head.as_deref();
        while let Some(n) = cur {
            print!("{}={} -> ", n.data, n.value);
            cur = n.next.as_deref();
        }
        println!("nullptr");
    }
}

impl Drop for UList {
    // Итеративное удаление, чтобы длинный список не переполнил стек рекурсией.
    fn drop(&mut self) {
        let mut cur = self.head.take();
        while let Some(mut n) = cur {
            cur = n.next.take();
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn s(v: &str) -> String {
        v.to_string()
    }

    fn keys(l: &UList) -> Vec<String> {
        l.get_pairs().into_iter().map(|p| p.0).collect()
    }

    fn filled() -> UList {
        let mut l = UList::new();
        for v in ["a", "b", "c"] {
            l.insert_tail(s(v));
        }
        l
    }

    #[test]
    fn insert_head_and_tail() {
        let mut l = UList::new();
        l.insert_tail(s("b"));
        l.insert_head(s("a"));
        l.insert_tail(s("c"));
        assert_eq!(keys(&l), vec!["a", "b", "c"]);
        assert_eq!(l.size(), 3);
    }

    #[test]
    fn insert_before_after() {
        let mut l = filled();
        l.insert_before("a", s("x"));
        l.insert_before("c", s("y"));
        l.insert_after("c", s("z"));
        l.insert_after("nope", s("bad"));
        l.insert_before("nope", s("bad"));
        assert_eq!(keys(&l), vec!["x", "a", "b", "y", "c", "z"]);
        assert_eq!(l.size(), 6);
    }

    #[test]
    fn delete_head_tail() {
        let mut l = filled();
        l.delete_head();
        l.delete_tail();
        assert_eq!(keys(&l), vec!["b"]);
        l.delete_tail();
        assert_eq!(l.size(), 0);
        l.delete_head();
        l.delete_tail();
        assert_eq!(l.size(), 0);
    }

    #[test]
    fn delete_before_after_key() {
        let mut l = filled();
        l.delete_before("a");
        assert_eq!(l.size(), 3);
        l.delete_before("c");
        assert_eq!(keys(&l), vec!["a", "c"]);
        l.delete_after("c");
        assert_eq!(l.size(), 2);
        l.delete_after("a");
        assert_eq!(keys(&l), vec!["a"]);
        l.delete_key("zzz");
        l.delete_key("a");
        assert_eq!(l.size(), 0);
        l.delete_before("a");
    }

    #[test]
    fn search_and_print() {
        let l = filled();
        assert_eq!(l.search("b"), Some(s("b")));
        assert_eq!(l.search("nope"), None);
        l.print_iter();
        l.print_rec();
        UList::new().print_iter();
        UList::new().print_rec();
    }

    #[test]
    fn key_value_api() {
        let mut l = UList::new();
        l.insert_head_kv(s("k1"), s("v1"));
        l.insert_head_kv(s("k2"), s("v2"));
        assert_eq!(l.get_value("k1"), Some(s("v1")));
        assert_eq!(l.get_value("zz"), None);
        assert!(l.update_value("k1", "new"));
        assert!(!l.update_value("zz", "x"));
        assert_eq!(l.get_value("k1"), Some(s("new")));
        assert_eq!(l.get_pairs(), vec![(s("k2"), s("v2")), (s("k1"), s("new"))]);
        assert!(l.remove_key("k2"));
        assert!(!l.remove_key("k2"));
        assert_eq!(l.size(), 1);
        l.print_pairs();
    }

    #[test]
    fn long_list_drop_does_not_overflow() {
        let mut l = UList::new();
        for i in 0..100_000 {
            l.insert_head(i.to_string());
        }
        assert_eq!(l.size(), 100_000);
    }
}
