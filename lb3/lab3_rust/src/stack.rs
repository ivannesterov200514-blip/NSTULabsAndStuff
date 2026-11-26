struct STNode {
    data: String,
    next: Option<Box<STNode>>,
}

#[derive(Default)]
pub struct Stack {
    top: Option<Box<STNode>>,
    sz: usize,
}

impl Stack {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn size(&self) -> usize {
        self.sz
    }

    pub fn push(&mut self, val: String) {
        let next = self.top.take();
        self.top = Some(Box::new(STNode { data: val, next }));
        self.sz += 1;
    }

    pub fn pop(&mut self) -> Option<String> {
        match self.top.take() {
            None => {
                println!("Ошибка: Стек пуст");
                None
            }
            Some(node) => {
                let node = *node;
                self.top = node.next;
                self.sz -= 1;
                println!("{}", node.data);
                Some(node.data)
            }
        }
    }

    pub fn print(&self) {
        if self.top.is_none() {
            println!("Стек пуст");
            return;
        }
        let mut cur = self.top.as_deref();
        while let Some(n) = cur {
            println!("{}", n.data);
            cur = n.next.as_deref();
        }
        println!("nullptr");
    }

    pub fn items(&self) -> Vec<String> {
        let mut res = Vec::new();
        let mut cur = self.top.as_deref();
        while let Some(n) = cur {
            res.push(n.data.clone());
            cur = n.next.as_deref();
        }
        res
    }
}

impl Drop for Stack {
    fn drop(&mut self) {
        let mut cur = self.top.take();
        while let Some(mut n) = cur {
            cur = n.next.take();
        }
    }
}

#[cfg(test)]
#[path = "../tests/unit/stack.rs"]
mod tests;