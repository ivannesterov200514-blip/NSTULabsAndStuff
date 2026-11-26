use lab3_rust::storage::{load_records, save_records, Record};
use std::fs;
use std::path::PathBuf;
use std::time::{SystemTime, UNIX_EPOCH};

fn test_path(extension: &str) -> PathBuf {
    let stamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_nanos();
    std::env::temp_dir().join(format!("lb3_{}_{}.{}", std::process::id(), stamp, extension))
}

fn make_record() -> Record {
    Record {
        kind: 'M',
        name: "array".to_string(),
        values: vec!["first".to_string(), "second".to_string()],
    }
}

#[test]
fn text_roundtrip() {
    let path = test_path("data");
    let records = vec![make_record()];
    save_records(path.to_str().unwrap(), &records).unwrap();
    assert_eq!(load_records(path.to_str().unwrap()).unwrap(), records);
    fs::remove_file(path).unwrap();
}

#[test]
fn binary_roundtrip_with_spaces() {
    let path = test_path("bin");
    let mut record = make_record();
    record.values.push("hello world".to_string());
    let records = vec![record];
    save_records(path.to_str().unwrap(), &records).unwrap();
    assert_eq!(load_records(path.to_str().unwrap()).unwrap(), records);
    fs::remove_file(path).unwrap();
}

#[test]
fn damaged_binary_is_rejected() {
    let path = test_path("bin");
    fs::write(&path, b"incorrect data").unwrap();
    assert!(load_records(path.to_str().unwrap()).is_err());
    fs::remove_file(path).unwrap();
}

#[test]
fn text_format_rejects_value_with_spaces() {
    let path = test_path("data");
    let records = vec![Record {
        kind: 'M',
        name: "array".to_string(),
        values: vec!["two words".to_string()],
    }];
    assert!(save_records(path.to_str().unwrap(), &records).is_err());
    assert!(!path.exists());
}

#[test]
fn binary_format_matches_cpp_header() {
    let path = test_path("bin");
    save_records(path.to_str().unwrap(), &[make_record()]).unwrap();
    let bytes = fs::read(&path).unwrap();
    assert_eq!(&bytes[0..4], b"LB3B");
    assert_eq!(&bytes[4..8], &[1, 0, 0, 0]);
    assert_eq!(&bytes[8..12], &[1, 0, 0, 0]);
    fs::remove_file(path).unwrap();
}