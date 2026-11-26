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