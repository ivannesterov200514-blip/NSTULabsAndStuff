use std::ptr;

struct QNode {
    data: String,
    next: Option<Box<QNode>>,
}

pub struct Queue {
    head: Option<Box<QNode>>,
    tail: *mut QNode,
    sz: usize,
}

impl Default for Queue {
    fn default() -> Self {
        Self { head: None, tail: ptr::null_mut(), sz: 0 }
    }
}

impl Queue {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn size(&self) -> usize {
        self.sz
    }

    pub fn push(&mut self, val: String) {
        let mut node = Box::new(QNode { data: val, next: None });
        let raw: *mut QNode = &mut *node;
        if self.tail.is_null() {
            self.head = Some(node);
        } else {

            unsafe {
                (*self.tail).next = Some(node);
            }
        }
        self.tail = raw;
        self.sz += 1;
    }

    pub fn pop(&mut self) -> Option<String> {
        match self.head.take() {
            None => {
                println!("Ошибка: Очередь пуста");
                None
            }
            Some(node) => {
                let node = *node;
                self.head = node.next;
                if self.head.is_none() {
                    self.tail = ptr::null_mut();
                }
                self.sz -= 1;
                println!("{}", node.data);
                Some(node.data)
            }
        }
    }

    pub fn print(&self) {
        if self.head.is_none() {
            println!("Очередь пуста");
            return;
        }
        print!("Head: ");
        let mut cur = self.head.as_deref();
        while let Some(n) = cur {
            print!("{} -> ", n.data);
            cur = n.next.as_deref();
        }
        println!(": Tail");
    }

    pub fn items(&self) -> Vec<String> {
        let mut res = Vec::new();
        let mut cur = self.head.as_deref();
        while let Some(n) = cur {
            res.push(n.data.clone());
            cur = n.next.as_deref();
        }
        res
    }
}

impl Drop for Queue {
    fn drop(&mut self) {
        self.tail = ptr::null_mut();
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

    #[test]
    fn fifo_order() {
        let mut q = Queue::new();
        q.push(s("1"));
        q.push(s("2"));
        q.push(s("3"));
        assert_eq!(q.size(), 3);
        assert_eq!(q.items(), vec!["1", "2", "3"]);
        assert_eq!(q.pop(), Some(s("1")));
        assert_eq!(q.pop(), Some(s("2")));
        assert_eq!(q.pop(), Some(s("3")));
        assert_eq!(q.size(), 0);
    }

    #[test]
    fn pop_empty_and_reuse() {
        let mut q = Queue::default();
        assert_eq!(q.pop(), None);
        q.print();
        q.push(s("a"));
        assert_eq!(q.pop(), Some(s("a")));
        assert_eq!(q.pop(), None);

        q.push(s("b"));
        q.push(s("c"));
        q.print();
        assert_eq!(q.items(), vec!["b", "c"]);
    }

    #[test]
    fn long_queue_drop_does_not_overflow() {
        let mut q = Queue::new();
        for i in 0..100_000 {
            q.push(i.to_string());
        }
        assert_eq!(q.size(), 100_000);
    }
}
