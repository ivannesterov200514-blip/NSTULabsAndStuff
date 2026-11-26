    use super::*;

    fn s(v: &str) -> String {
        v.to_string()
    }

    #[test]
    fn hash_functions_match_cpp() {

        assert_eq!(hash1("", 101), 0);
        assert_eq!(hash2("", 101), 7);
        assert_eq!(hash1("a", 101), 97 % 101);
        assert_eq!(hash2("a", 101), (7 * 131 + 97) % 101);
        assert_eq!(hash1("apple", 101), 29);
        assert_eq!(hash1("abc", 0), 0);
        assert_eq!(hash2("abc", 0), 0);
    }

    #[test]
    fn chain_put_get_update() {
        let mut t = ChainTable::default();
        t.put(s("apple"), s("red"));
        t.put(s("banana"), s("yellow"));
        t.put(s("apple"), s("green"));
        assert_eq!(t.size(), 2);
        assert_eq!(t.get("apple"), Some(s("green")));
        assert_eq!(t.get("nope"), None);
        assert_eq!(t.capacity(), 101);
    }

    #[test]
    fn chain_remove_with_collisions() {
        let mut t = ChainTable::new(1);
        for i in 0..10 {
            t.put(format!("k{}", i), format!("v{}", i));
        }
        assert_eq!(t.size(), 10);
        assert!(t.remove("k5"));
        assert!(!t.remove("k5"));
        assert!(t.remove("k0"));
        assert!(t.remove("k9"));
        assert_eq!(t.size(), 7);
        assert_eq!(t.get("k5"), None);
        assert_eq!(t.get("k4"), Some(s("v4")));
        assert_eq!(t.get_all().len(), 7);
    }

    #[test]
    fn chain_zero_capacity_and_print() {
        let mut t = ChainTable::new(0);
        assert_eq!(t.capacity(), 101);
        t.print();
        t.put(s("a"), s("1"));
        t.print();
    }

    #[test]
    fn cuckoo_put_get_update_remove() {
        let mut t = CuckooTable::default();
        t.put(s("a"), s("1"));
        t.put(s("b"), s("2"));
        t.put(s("a"), s("3"));
        assert_eq!(t.size(), 2);
        assert_eq!(t.get("a"), Some(s("3")));
        assert_eq!(t.get("zzz"), None);
        assert!(t.remove("a"));
        assert!(!t.remove("a"));
        assert_eq!(t.size(), 1);
        t.print();
    }

    #[test]
    fn cuckoo_empty_key_is_ignored() {
        let mut t = CuckooTable::new(0);
        t.put(s(""), s("x"));
        assert_eq!(t.size(), 0);
        assert_eq!(t.get(""), None);
        assert!(!t.remove(""));
        t.print();
    }

    #[test]
    fn cuckoo_grows_and_keeps_everything() {
        let mut t = CuckooTable::new(3);
        for i in 0..500 {
            t.put(format!("key{}", i), format!("val{}", i));
        }
        assert_eq!(t.size(), 500);
        assert!(t.grows() > 0);
        assert!(t.capacity() > 3);
        for i in 0..500 {
            assert_eq!(t.get(&format!("key{}", i)), Some(format!("val{}", i)));
        }
        for i in (0..500).step_by(2) {
            assert!(t.remove(&format!("key{}", i)));
        }
        assert_eq!(t.size(), 250);
        assert_eq!(t.get_all().len(), 250);
        assert!(t.kicks() > 0);
    }