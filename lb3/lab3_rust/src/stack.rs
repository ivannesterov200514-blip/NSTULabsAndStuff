struct STNode {
    data: String,
    next: Option<Box<STNode>>,
}

/// Стек на односвязном списке (аналог `Stack` из C++).
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

    /// Снимает вершину, печатает её значение и возвращает его.
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

    /// Элементы от вершины ко дну.
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
mod tests {
    use super::*;

    fn s(v: &str) -> String {
        v.to_string()
    }

    #[test]
    fn lifo_order() {
        let mut st = Stack::new();
        st.push(s("1"));
        st.push(s("2"));
        st.push(s("3"));
        assert_eq!(st.size(), 3);
        assert_eq!(st.items(), vec!["3", "2", "1"]);
        assert_eq!(st.pop(), Some(s("3")));
        assert_eq!(st.pop(), Some(s("2")));
        assert_eq!(st.pop(), Some(s("1")));
        assert_eq!(st.size(), 0);
    }

    #[test]
    fn pop_empty() {
        let mut st = Stack::default();
        assert_eq!(st.pop(), None);
        st.print();
        st.push(s("x"));
        st.print();
    }

    #[test]
    fn long_stack_drop_does_not_overflow() {
        let mut st = Stack::new();
        for i in 0..100_000 {
            st.push(i.to_string());
        }
        assert_eq!(st.size(), 100_000);
    }
}
