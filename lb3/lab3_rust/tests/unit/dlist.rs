    use super::*;

    fn s(v: &str) -> String {
        v.to_string()
    }

    fn filled() -> DList {
        let mut l = DList::new();
        for v in ["a", "b", "c"] {
            l.insert_tail(s(v));
        }
        l
    }

    fn items_backwards(l: &DList) -> Vec<String> {
        let mut res = Vec::new();
        let mut cur = l.tail.clone();
        while let Some(n) = cur {
            res.push(n.borrow().data.clone());
            cur = n.borrow().prev.as_ref().and_then(|w| w.upgrade());
        }
        res
    }

    fn assert_consistent(l: &DList) {
        let mut back = items_backwards(l);
        back.reverse();
        assert_eq!(l.items(), back);
        assert_eq!(l.items().len(), l.size());
    }

    #[test]
    fn insert_head_tail() {
        let mut l = DList::new();
        l.insert_tail(s("b"));
        l.insert_head(s("a"));
        l.insert_tail(s("c"));
        assert_eq!(l.items(), vec!["a", "b", "c"]);
        assert_consistent(&l);
    }

    #[test]
    fn insert_before_after() {
        let mut l = filled();
        l.insert_before("a", s("x"));
        l.insert_before("c", s("y"));
        l.insert_after("c", s("z"));
        l.insert_after("a", s("w"));
        l.insert_before("nope", s("bad"));
        l.insert_after("nope", s("bad"));
        assert_eq!(l.items(), vec!["x", "a", "w", "b", "y", "c", "z"]);
        assert_consistent(&l);
    }

    #[test]
    fn delete_head_tail() {
        let mut l = filled();
        l.delete_head();
        l.delete_tail();
        assert_eq!(l.items(), vec!["b"]);
        assert_consistent(&l);
        l.delete_tail();
        assert_eq!(l.size(), 0);
        l.delete_head();
        l.delete_tail();
        assert_eq!(l.size(), 0);
        l.insert_tail(s("again"));
        assert_eq!(l.items(), vec!["again"]);
        assert_consistent(&l);
    }

    #[test]
    fn delete_before_after_key() {
        let mut l = filled();
        l.delete_before("a");
        l.delete_after("c");
        l.delete_before("zzz");
        l.delete_after("zzz");
        assert_eq!(l.size(), 3);
        l.delete_before("c");
        assert_eq!(l.items(), vec!["a", "c"]);
        assert_consistent(&l);
        l.delete_after("a");
        assert_eq!(l.items(), vec!["a"]);
        l.delete_key("zzz");
        l.delete_key("a");
        assert_eq!(l.size(), 0);
        assert_consistent(&l);
    }

    #[test]
    fn search_and_print() {
        let l = filled();
        assert_eq!(l.search("b"), Some(s("b")));
        assert_eq!(l.search("nope"), None);
        l.print_iter();
        l.print_rec();
        DList::new().print_iter();
        DList::new().print_rec();
    }

    #[test]
    fn long_list_drop_does_not_overflow() {
        let mut l = DList::new();
        for i in 0..100_000 {
            l.insert_tail(i.to_string());
        }
        assert_eq!(l.size(), 100_000);
    }