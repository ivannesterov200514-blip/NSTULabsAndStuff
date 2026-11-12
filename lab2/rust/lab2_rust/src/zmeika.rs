use std::collections::VecDeque;

#[derive(Debug, Clone)]
pub struct TNode {
    pub key: i32,
    pub left: Option<Box<TNode>>,
    pub right: Option<Box<TNode>>,
    height: i32,
}

impl TNode {
    fn new(key: i32) -> Self {
        Self { key, left: None, right: None, height: 1 }
    }
}

#[derive(Debug, Clone, Default)]
pub struct AvlTree {
    pub root: Option<Box<TNode>>,
}

fn height(node: &Option<Box<TNode>>) -> i32 {
    node.as_ref().map_or(0, |n| n.height)
}

fn update_height(node: &mut Box<TNode>) {
    node.height = 1 + height(&node.left).max(height(&node.right));
}

fn balance_factor(node: &TNode) -> i32 {
    height(&node.left) - height(&node.right)
}

fn rotate_right(mut y: Box<TNode>) -> Box<TNode> {
    let mut x = y.left.take().expect("правый поворот требует левого потомка");
    y.left = x.right.take();
    update_height(&mut y);
    x.right = Some(y);
    update_height(&mut x);
    x
}

fn rotate_left(mut x: Box<TNode>) -> Box<TNode> {
    let mut y = x.right.take().expect("левый поворот требует правого потомка");
    x.right = y.left.take();
    update_height(&mut x);
    y.left = Some(x);
    update_height(&mut y);
    y
}

fn insert_node(node: Option<Box<TNode>>, key: i32) -> Option<Box<TNode>> {
    let mut root = match node {
        None => return Some(Box::new(TNode::new(key))),
        Some(node) => node,
    };

    if key < root.key {
        root.left = insert_node(root.left.take(), key);
    } else if key > root.key {
        root.right = insert_node(root.right.take(), key);
    } else {
        return Some(root);
    }

    update_height(&mut root);
    let balance = balance_factor(&root);

    if balance > 1 && key < root.left.as_ref().expect("левый потомок").key {
        return Some(rotate_right(root));
    }
    if balance < -1 && key > root.right.as_ref().expect("правый потомок").key {
        return Some(rotate_left(root));
    }
    if balance > 1 && key > root.left.as_ref().expect("левый потомок").key {
        root.left = root.left.take().map(rotate_left);
        return Some(rotate_right(root));
    }
    if balance < -1 && key < root.right.as_ref().expect("правый потомок").key {
        root.right = root.right.take().map(rotate_right);
        return Some(rotate_left(root));
    }
    Some(root)
}

impl AvlTree {
    pub fn insert(&mut self, key: i32) {
        self.root = insert_node(self.root.take(), key);
    }

    fn levels(&self) -> Vec<Vec<i32>> {
        let mut levels = Vec::new();
        let Some(root) = self.root.as_deref() else {
            return levels;
        };
        let mut queue = VecDeque::new();
        queue.push_back(root);
        while !queue.is_empty() {
            let count = queue.len();
            let mut level = Vec::with_capacity(count);
            for _ in 0..count {
                let node = queue.pop_front().expect("элемент очереди");
                level.push(node.key);
                if let Some(left) = node.left.as_deref() {
                    queue.push_back(left);
                }
                if let Some(right) = node.right.as_deref() {
                    queue.push_back(right);
                }
            }
            levels.push(level);
        }
        levels
    }

    pub fn print_wide(&self) {
        let levels = self.levels();
        if levels.is_empty() {
            println!("Дерево пусто");
            return;
        }
        for (index, level) in levels.iter().enumerate() {
            print!("Уровень {}: ", index + 1);
            for key in level {
                print!("{} ", key);
            }
            println!();
        }
    }

    pub fn print_snake(&self) {
        let mut levels = self.levels();
        if levels.is_empty() {
            println!("Дерево пусто");
            return;
        }
        let mut first = true;
        for (index, level) in levels.iter_mut().enumerate() {
            if index % 2 == 1 {
                level.reverse();
            }
            for key in level {
                if !first {
                    print!(" – ");
                }
                print!("{}", key);
                first = false;
            }
        }
        println!();
    }

    pub fn breadth_first_keys(&self) -> Vec<i32> {
        self.levels().into_iter().flatten().collect()
    }
}
