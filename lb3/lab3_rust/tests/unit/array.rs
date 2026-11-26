    use super::*;

    fn s(v: &str) -> String {
        v.to_string()
    }

    #[test]
    fn insert_and_search() {
        let mut a = Arr::default();
        a.insert_tail(s("a"));
        a.insert_tail(s("b"));
        a.insert_tail(s("c"));
        assert_eq!(a.size(), 3);
        assert_eq!(a.search(1), Some(s("b")));
        assert_eq!(a.search(3), None);
        assert_eq!(a.search(-1), None);
    }

    #[test]
    fn insert_by_index() {
        let mut a = Arr::new(1);
        assert!(a.insert_idx(s("x"), 0));
        assert!(a.insert_idx(s("z"), 1));
        assert!(a.insert_idx(s("y"), 1));
        assert!(!a.insert_idx(s("bad"), 5));
        assert!(!a.insert_idx(s("bad"), -1));
        assert_eq!(a.items(), vec![s("x"), s("y"), s("z")]);
    }

    #[test]
    fn delete_and_change() {
        let mut a = Arr::new(0);
        for v in ["1", "2", "3", "4"] {
            a.insert_tail(s(v));
        }
        assert!(a.delete_idx(1));
        assert!(!a.delete_idx(3));
        assert!(a.change_idx(s("9"), 0));
        assert!(!a.change_idx(s("9"), 10));
        assert_eq!(a.items(), vec![s("9"), s("3"), s("4")]);
        assert!(a.capacity() >= 3);
    }

    #[test]
    fn print_does_not_panic() {
        let mut a = Arr::default();
        a.print();
        a.insert_tail(s("q"));
        a.print();
    }