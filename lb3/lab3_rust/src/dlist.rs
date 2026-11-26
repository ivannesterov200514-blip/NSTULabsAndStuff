use std::cell::RefCell;
use std::rc::{Rc, Weak};

type Link = Option<Rc<RefCell<SNode>>>;

struct SNode {
    data: String,
    next: Link,
    prev: Option<Weak<RefCell<SNode>>>,
}

#[derive(Default)]
pub struct DList {
    head: Link,
    tail: Link,
    sz: usize,
}

fn new_node(data: String) -> Rc<RefCell<SNode>> {
    Rc::new(RefCell::new(SNode { data, next: None, prev: None }))
}

impl DList {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn size(&self) -> usize {
        self.sz
    }

    fn find(&self, key: &str) -> Option<Rc<RefCell<SNode>>> {
        let mut cur = self.head.clone();
        while let Some(n) = cur {
            if n.borrow().data.as_str() == key {
                return Some(n);
            }
            cur = n.borrow().next.clone();
        }
        None
    }

    fn unlink(&mut self, v: &Rc<RefCell<SNode>>) {
        let (prev, next) = {
            let mut node = v.borrow_mut();
            let prev = node.prev.take().and_then(|w| w.upgrade());
            let next = node.next.take();
            (prev, next)
        };
        match &prev {
            Some(p) => p.borrow_mut().next = next.clone(),
            None => self.head = next.clone(),
        }
        match &next {
            Some(n) => n.borrow_mut().prev = prev.as_ref().map(Rc::downgrade),
            None => self.tail = prev.clone(),
        }
        self.sz -= 1;
    }

    pub fn insert_head(&mut self, val: String) {
        let n = new_node(val);
        match self.head.take() {
            Some(h) => {
                h.borrow_mut().prev = Some(Rc::downgrade(&n));
                n.borrow_mut().next = Some(h);
            }
            None => self.tail = Some(n.clone()),
        }
        self.head = Some(n);
        self.sz += 1;
    }

    pub fn insert_tail(&mut self, val: String) {
        let n = new_node(val);
        match self.tail.take() {
            Some(t) => {
                n.borrow_mut().prev = Some(Rc::downgrade(&t));
                t.borrow_mut().next = Some(n.clone());
            }
            None => self.head = Some(n.clone()),
        }
        self.tail = Some(n);
        self.sz += 1;
    }

    pub fn insert_before(&mut self, key: &str, val: String) {
        let cur = match self.find(key) {
            Some(c) => c,
            None => {
                println!("Ошибка: Не найдено");
                return;
            }
        };
        let n = new_node(val);
        let prev = cur.borrow_mut().prev.take().and_then(|w| w.upgrade());
        n.borrow_mut().next = Some(cur.clone());
        cur.borrow_mut().prev = Some(Rc::downgrade(&n));
        match prev {
            Some(p) => {
                n.borrow_mut().prev = Some(Rc::downgrade(&p));
                p.borrow_mut().next = Some(n);
            }
            None => self.head = Some(n),
        }
        self.sz += 1;
    }

    pub fn insert_after(&mut self, key: &str, val: String) {
        let cur = match self.find(key) {
            Some(c) => c,
            None => {
                println!("Ошибка: Не найдено");
                return;
            }
        };
        let n = new_node(val);
        let next = cur.borrow_mut().next.take();
        n.borrow_mut().prev = Some(Rc::downgrade(&cur));
        match next {
            Some(nx) => {
                nx.borrow_mut().prev = Some(Rc::downgrade(&n));
                n.borrow_mut().next = Some(nx);
            }
            None => self.tail = Some(n.clone()),
        }
        cur.borrow_mut().next = Some(n);
        self.sz += 1;
    }

    pub fn delete_head(&mut self) {
        let h = match self.head.clone() {
            Some(h) => h,
            None => {
                println!("Ошибка: Список пуст");
                return;
            }
        };
        if self.sz == 1 {
            println!("Последний узел списка удален.");
        }
        self.unlink(&h);
    }

    pub fn delete_tail(&mut self) {
        let t = match self.tail.clone() {
            Some(t) => t,
            None => {
                println!("Ошибка: Список пуст");
                return;
            }
        };
        if self.sz == 1 {
            println!("Последний узел списка удален.");
        }
        self.unlink(&t);
    }

    pub fn delete_before(&mut self, key: &str) {
        let cur = match self.find(key) {
            Some(c) => c,
            None => {
                println!("Ошибка: Не найдено");
                return;
            }
        };
        let prev = cur.borrow().prev.as_ref().and_then(|w| w.upgrade());
        match prev {
            Some(p) => self.unlink(&p),
            None => println!("Ошибка: Перед элементом ничего нет"),
        }
    }

    pub fn delete_after(&mut self, key: &str) {
        let cur = match self.find(key) {
            Some(c) => c,
            None => {
                println!("Ошибка: Не найдено");
                return;
            }
        };
        let next = cur.borrow().next.clone();
        match next {
            Some(nx) => self.unlink(&nx),
            None => println!("Ошибка: После элемента ничего нет"),
        }
    }

    pub fn delete_key(&mut self, key: &str) {
        let cur = match self.find(key) {
            Some(c) => c,
            None => {
                println!("Ошибка: Не найдено");
                return;
            }
        };
        if self.sz == 1 {
            println!("Последний узел списка удален.");
        }
        self.unlink(&cur);
    }

    pub fn search(&self, key: &str) -> Option<String> {
        match self.find(key) {
            Some(n) => {
                let data = n.borrow().data.clone();
                println!("{}", data);
                Some(data)
            }
            None => {
                println!("Ошибка: Не найдено");
                None
            }
        }
    }

    pub fn items(&self) -> Vec<String> {
        let mut res = Vec::new();
        let mut cur = self.head.clone();
        while let Some(n) = cur {
            res.push(n.borrow().data.clone());
            cur = n.borrow().next.clone();
        }
        res
    }

    pub fn print_iter(&self) {
        if self.head.is_none() {
            println!("Список пуст");
            return;
        }
        print!("nullptr <-> ");
        for v in self.items() {
            print!("{} <-> ", v);
        }
        println!("nullptr");
    }

    fn print_rec_node(n: &Link) {
        match n {
            None => println!("nullptr"),
            Some(node) => {
                print!("{} <-> ", node.borrow().data);
                let next = node.borrow().next.clone();
                Self::print_rec_node(&next);
            }
        }
    }

    pub fn print_rec(&self) {
        if self.head.is_none() {
            println!("Список пуст");
            return;
        }
        print!("nullptr <-> ");
        Self::print_rec_node(&self.head);
    }


}

impl Drop for DList {

    fn drop(&mut self) {
        self.tail = None;
        let mut cur = self.head.take();
        while let Some(n) = cur {
            cur = n.borrow_mut().next.take();
        }
    }
}

#[cfg(test)]
#[path = "../tests/unit/dlist.rs"]
mod tests;