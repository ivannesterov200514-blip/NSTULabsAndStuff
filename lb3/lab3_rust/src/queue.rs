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
#[path = "../tests/unit/queue.rs"]
mod tests;