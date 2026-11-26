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

    fn drop(&mut self) {
        let mut cur = self.head.take();
        while let Some(mut n) = cur {
            cur = n.next.take();
        }
    }
}

#[cfg(test)]
#[path = "../tests/unit/ulist.rs"]
mod tests;