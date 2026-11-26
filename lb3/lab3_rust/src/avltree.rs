use std::collections::VecDeque;

struct TNode {
    key: i32,
    left: Option<Box<TNode>>,
    right: Option<Box<TNode>>,
    height: i32,
}

impl TNode {
    fn new(key: i32) -> Self {
        Self { key, left: None, right: None, height: 1 }
    }
}

fn height(t: &Option<Box<TNode>>) -> i32 {
    match t {
        Some(n) => n.height,
        None => 0,
    }
}

fn balance_of(n: &TNode) -> i32 {
    height(&n.left) - height(&n.right)
}

fn balance(t: &Option<Box<TNode>>) -> i32 {
    match t {
        Some(n) => balance_of(n),
        None => 0,
    }
}

fn update(n: &mut TNode) {
    n.height = 1 + height(&n.left).max(height(&n.right));
}

fn rotate_right(mut y: Box<TNode>) -> Box<TNode> {
    let mut x = y.left.take().expect("у правого поворота есть левый потомок");
    y.left = x.right.take();
    update(&mut y);
    x.right = Some(y);
    update(&mut x);
    x
}

fn rotate_left(mut x: Box<TNode>) -> Box<TNode> {
    let mut y = x.right.take().expect("у левого поворота есть правый потомок");
    x.right = y.left.take();
    update(&mut x);
    y.left = Some(x);
    update(&mut y);
    y
}

fn rebalance(mut n: Box<TNode>) -> Box<TNode> {
    update(&mut n);
    let b = balance_of(&n);
    if b > 1 {
        if balance(&n.left) < 0 {
            n.left = Some(rotate_left(n.left.take().expect("левый потомок")));
        }
        return rotate_right(n);
    }
    if b < -1 {
        if balance(&n.right) > 0 {
            n.right = Some(rotate_right(n.right.take().expect("правый потомок")));
        }
        return rotate_left(n);
    }
    n
}

fn insert_node(t: Option<Box<TNode>>, val: i32) -> Option<Box<TNode>> {
    let mut n = match t {
        None => return Some(Box::new(TNode::new(val))),
        Some(n) => n,
    };
    if val < n.key {
        n.left = insert_node(n.left.take(), val);
    } else if val > n.key {
        n.right = insert_node(n.right.take(), val);
    } else {
        return Some(n);
    }
    Some(rebalance(n))
}

fn min_key(t: &TNode) -> i32 {
    let mut cur = t;
    while let Some(l) = cur.left.as_deref() {
        cur = l;
    }
    cur.key
}

fn delete_node(t: Option<Box<TNode>>, val: i32) -> Option<Box<TNode>> {
    let mut n = t?;
    if val < n.key {
        n.left = delete_node(n.left.take(), val);
    } else if val > n.key {
        n.right = delete_node(n.right.take(), val);
    } else {
        match (n.left.take(), n.right.take()) {
            (None, r) => return r,
            (l, None) => return l,
            (l, Some(r)) => {
                let m = min_key(&r);
                n.key = m;
                n.left = l;
                n.right = delete_node(Some(r), m);
            }
        }
    }
    Some(rebalance(n))
}

fn search_node(t: &Option<Box<TNode>>, val: i32) -> bool {
    let mut cur = t.as_deref();
    while let Some(n) = cur {
        if val == n.key {
            return true;
        }
        cur = if val < n.key { n.left.as_deref() } else { n.right.as_deref() };
    }
    false
}

fn print_level(t: &Option<Box<TNode>>, level: i32) {
    if let Some(n) = t {
        if level == 1 {
            print!("{} ", n.key);
            return;
        }
        print_level(&n.left, level - 1);
        print_level(&n.right, level - 1);
    }
}

#[derive(Default)]
pub struct AVLTree {
    root: Option<Box<TNode>>,
}

impl AVLTree {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn height(&self) -> i32 {
        height(&self.root)
    }

    pub fn insert(&mut self, val: i32) {
        self.root = insert_node(self.root.take(), val);
    }

    pub fn delete(&mut self, val: i32) -> bool {
        if !self.search(val) {
            println!("Ошибка: Не найдено");
            return false;
        }
        self.root = delete_node(self.root.take(), val);
        true
    }

    pub fn search(&self, val: i32) -> bool {
        search_node(&self.root, val)
    }

    pub fn print_wide(&self) {
        if self.root.is_none() {
            println!("Дерево пусто");
            return;
        }
        for level in 1..=self.height() {
            print!("Уровень {}: ", level);
            print_level(&self.root, level);
            println!();
        }
    }

    pub fn items(&self) -> Vec<i32> {
        let mut res = Vec::new();
        let mut q: VecDeque<&TNode> = VecDeque::new();
        if let Some(r) = self.root.as_deref() {
            q.push_back(r);
        }
        while let Some(n) = q.pop_front() {
            res.push(n.key);
            if let Some(l) = n.left.as_deref() {
                q.push_back(l);
            }
            if let Some(r) = n.right.as_deref() {
                q.push_back(r);
            }
        }
        res
    }
}

#[cfg(test)]
#[path = "../tests/unit/avltree.rs"]
mod tests;