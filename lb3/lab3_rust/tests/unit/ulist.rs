    use super::*;

    fn s(v: &str) -> String {
        v.to_string()
    }

    fn keys(l: &UList) -> Vec<String> {
        l.get_pairs().into_iter().map(|p| p.0).collect()
    }

    fn filled() -> UList {
        let mut l = UList::new();
        for v in ["a", "b", "c"] {
            l.insert_tail(s(v));
        }
        l
    }

    #[test]
    fn insert_head_and_tail() {
        let mut l = UList::new();
        l.insert_tail(s("b"));
        l.insert_head(s("a"));
        l.insert_tail(s("c"));
        assert_eq!(keys(&l), vec!["a", "b", "c"]);
        assert_eq!(l.size(), 3);
    }

    #[test]
    fn insert_before_after() {
        let mut l = filled();
        l.insert_before("a", s("x"));
        l.insert_before("c", s("y"));
        l.insert_after("c", s("z"));
        l.insert_after("nope", s("bad"));
        l.insert_before("nope", s("bad"));
        assert_eq!(keys(&l), vec!["x", "a", "b", "y", "c", "z"]);
        assert_eq!(l.size(), 6);
    }

    #[test]
    fn delete_head_tail() {
        let mut l = filled();
        l.delete_head();
        l.delete_tail();
        assert_eq!(keys(&l), vec!["b"]);
        l.delete_tail();
        assert_eq!(l.size(), 0);
        l.delete_head();
        l.delete_tail();
        assert_eq!(l.size(), 0);
    }

    #[test]
    fn delete_before_after_key() {
        let mut l = filled();
        l.delete_before("a");
        assert_eq!(l.size(), 3);
        l.delete_before("c");
        assert_eq!(keys(&l), vec!["a", "c"]);
        l.delete_after("c");
        assert_eq!(l.size(), 2);
        l.delete_after("a");
        assert_eq!(keys(&l), vec!["a"]);
        l.delete_key("zzz");
        l.delete_key("a");
        assert_eq!(l.size(), 0);
        l.delete_before("a");
    }

    #[test]
    fn search_and_print() {
        let l = filled();
        assert_eq!(l.search("b"), Some(s("b")));
        assert_eq!(l.search("nope"), None);
        l.print_iter();
        l.print_rec();
        UList::new().print_iter();
        UList::new().print_rec();
    }

    #[test]
    fn key_value_api() {
        let mut l = UList::new();
        l.insert_head_kv(s("k1"), s("v1"));
        l.insert_head_kv(s("k2"), s("v2"));
        assert_eq!(l.get_value("k1"), Some(s("v1")));
        assert_eq!(l.get_value("zz"), None);
        assert!(l.update_value("k1", "new"));
        assert!(!l.update_value("zz", "x"));
        assert_eq!(l.get_value("k1"), Some(s("new")));
        assert_eq!(l.get_pairs(), vec![(s("k2"), s("v2")), (s("k1"), s("new"))]);
        assert!(l.remove_key("k2"));
        assert!(!l.remove_key("k2"));
        assert_eq!(l.size(), 1);
        l.print_pairs();
    }

    #[test]
    fn long_list_drop_does_not_overflow() {
        let mut l = UList::new();
        for i in 0..100_000 {
            l.insert_head(i.to_string());
        }
        assert_eq!(l.size(), 100_000);
    }