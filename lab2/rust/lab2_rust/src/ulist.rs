#[derive(Debug, Clone, Default)]
pub struct UList {
    head: Option<Box<Node>>,
    pub size: usize,
}

#[derive(Debug, Clone)]
struct Node {
    data: String,
    next: Option<Box<Node>>,
}

pub struct UListIter<'a> {
    next: Option<&'a Node>,
}

impl<'a> Iterator for UListIter<'a> {
    type Item = &'a String;

    fn next(&mut self) -> Option<Self::Item> {
        let node = self.next?;
        self.next = node.next.as_deref();
        Some(&node.data)
    }
}

impl UList {
    pub fn insert_head(&mut self, value: String) {
        let next = self.head.take();
        self.head = Some(Box::new(Node { data: value, next }));
        self.size += 1;
    }

    pub fn contains(&self, key: &str) -> bool {
        let mut current = self.head.as_deref();
        while let Some(node) = current {
            if node.data.as_str() == key {
                return true;
            }
            current = node.next.as_deref();
        }
        false
    }

    pub fn remove_key(&mut self, key: &str) -> bool {
        let mut link = &mut self.head;
        loop {
            let matches = match link.as_ref() {
                Some(node) => node.data.as_str() == key,
                None => return false,
            };
            if matches {
                let mut removed = link.take().expect("узел найден");
                *link = removed.next.take();
                self.size -= 1;
                return true;
            }
            let node = link.as_mut().expect("узел существует");
            link = &mut node.next;
        }
    }

    pub fn iter(&self) -> UListIter<'_> {
        UListIter { next: self.head.as_deref() }
    }
}
