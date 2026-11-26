    use super::*;

    fn check(t: &Option<Box<TNode>>, lo: Option<i32>, hi: Option<i32>) -> (bool, i32) {
        match t {
            None => (true, 0),
            Some(n) => {
                let in_range = !matches!(lo, Some(l) if n.key <= l) && !matches!(hi, Some(h) if n.key >= h);
                let (lok, lh) = check(&n.left, lo, Some(n.key));
                let (rok, rh) = check(&n.right, Some(n.key), hi);
                let ok = in_range
                    && lok
                    && rok
                    && (lh - rh).abs() <= 1
                    && n.height == 1 + lh.max(rh);
                (ok, 1 + lh.max(rh))
            }
        }
    }

    fn assert_valid(tree: &AVLTree) {
        assert!(check(&tree.root, None, None).0);
    }

    #[test]
    fn insert_search_ascending() {
        let mut t = AVLTree::new();
        for i in 1..=1000 {
            t.insert(i);
        }
        assert_valid(&t);
        assert!(t.height() <= 14);
        assert!(t.search(1) && t.search(500) && t.search(1000));
        assert!(!t.search(0) && !t.search(1001));
    }

    #[test]
    fn four_rotation_cases() {
        for order in [[3, 2, 1], [1, 2, 3], [3, 1, 2], [1, 3, 2]] {
            let mut t = AVLTree::new();
            for k in order {
                t.insert(k);
            }
            assert_valid(&t);
            assert_eq!(t.items(), vec![2, 1, 3]);
        }
    }

    #[test]
    fn duplicates_are_ignored() {
        let mut t = AVLTree::new();
        t.insert(5);
        t.insert(5);
        assert_eq!(t.items(), vec![5]);
    }

    #[test]
    fn delete_keeps_tree_valid() {
        let mut t = AVLTree::new();
        for i in 1..=200 {
            t.insert((i * 37) % 211);
        }
        let all = t.items();
        for (i, k) in all.iter().enumerate() {
            if i % 2 == 0 {
                assert!(t.delete(*k));
                assert_valid(&t);
            }
        }
        for (i, k) in all.iter().enumerate() {
            assert_eq!(t.search(*k), i % 2 == 1);
        }
    }

    #[test]
    fn delete_cases() {
        let mut t = AVLTree::new();
        assert!(!t.delete(1));
        for k in [10, 5, 15, 3, 7, 12, 20] {
            t.insert(k);
        }
        assert!(t.delete(3)); 
        assert!(t.delete(5)); 
        assert!(t.delete(10)); 
        assert!(!t.delete(10));
        assert_valid(&t);
        t.print_wide();
        AVLTree::new().print_wide();
    }

    #[test]
    fn items_are_level_order() {
        let mut t = AVLTree::new();
        for k in [5, 3, 8, 1, 4, 9, 10] {
            t.insert(k);
        }
        assert_eq!(t.items(), vec![5, 3, 9, 1, 4, 8, 10]);
    }