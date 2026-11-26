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