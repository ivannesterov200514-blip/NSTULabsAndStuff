use std::fs::{self, File};
use std::io::{self, ErrorKind, Read, Write};

const MAX_RECORDS: usize = 1_000_000;
const MAX_STRING_SIZE: usize = 1024 * 1024;

#[derive(Clone, Debug, PartialEq, Eq)]
pub struct Record {
    pub kind: char,
    pub name: String,
    pub values: Vec<String>,
}

fn valid_type(kind: char) -> bool {
    matches!(kind, 'M' | 'F' | 'L' | 'S' | 'Q' | 'T' | 'C' | 'K')
}

fn valid_record(record: &Record) -> bool {
    valid_type(record.kind)
        && !record.name.is_empty()
        && record.name.len() <= MAX_STRING_SIZE
        && record.values.len() <= MAX_RECORDS
        && record.values.iter().all(|v| v.len() <= MAX_STRING_SIZE)
        && (!matches!(record.kind, 'C' | 'K') || record.values.len() % 2 == 0)
}

fn invalid_file() -> io::Error {
    io::Error::new(ErrorKind::InvalidData, "Некорректный формат файла")
}

fn write_number(file: &mut File, value: u32) -> io::Result<()> {
    file.write_all(&value.to_le_bytes())
}

fn read_number(file: &mut File) -> io::Result<u32> {
    let mut bytes = [0u8; 4];
    file.read_exact(&mut bytes)?;
    Ok(u32::from_le_bytes(bytes))
}

fn write_string(file: &mut File, value: &str) -> io::Result<()> {
    if value.len() > MAX_STRING_SIZE {
        return Err(invalid_file());
    }
    write_number(file, value.len() as u32)?;
    file.write_all(value.as_bytes())
}

fn read_string(file: &mut File) -> io::Result<String> {
    let size = read_number(file)? as usize;
    if size > MAX_STRING_SIZE {
        return Err(invalid_file());
    }
    let mut bytes = vec![0; size];
    file.read_exact(&mut bytes)?;
    String::from_utf8(bytes).map_err(|_| invalid_file())
}

fn save_binary(path: &str, records: &[Record]) -> io::Result<()> {
    if records.len() > MAX_RECORDS {
        return Err(invalid_file());
    }
    if !records.iter().all(valid_record) {
        return Err(invalid_file());
    }
    let mut file = File::create(path)?;
    file.write_all(b"LB3B")?;
    write_number(&mut file, 1)?;
    write_number(&mut file, records.len() as u32)?;
    for record in records {
        file.write_all(&[record.kind as u8])?;
        write_string(&mut file, &record.name)?;
        write_number(&mut file, record.values.len() as u32)?;
        for value in &record.values {
            write_string(&mut file, value)?;
        }
    }
    file.flush()
}

fn load_binary(path: &str) -> io::Result<Vec<Record>> {
    let mut file = File::open(path)?;
    let mut header = [0u8; 4];
    file.read_exact(&mut header)?;
    if &header != b"LB3B" || read_number(&mut file)? != 1 {
        return Err(invalid_file());
    }
    let count = read_number(&mut file)? as usize;
    if count > MAX_RECORDS {
        return Err(invalid_file());
    }
    let mut records = Vec::with_capacity(count);
    for _ in 0..count {
        let mut kind = [0u8; 1];
        file.read_exact(&mut kind)?;
        let name = read_string(&mut file)?;
        let value_count = read_number(&mut file)? as usize;
        if value_count > MAX_RECORDS {
            return Err(invalid_file());
        }
        let mut values = Vec::with_capacity(value_count);
        for _ in 0..value_count {
            values.push(read_string(&mut file)?);
        }
        let record = Record { kind: kind[0] as char, name, values };
        if !valid_record(&record) {
            return Err(invalid_file());
        }
        records.push(record);
    }
    let mut extra = [0u8; 1];
    if file.read(&mut extra)? != 0 {
        return Err(invalid_file());
    }
    Ok(records)
}

fn save_text(path: &str, records: &[Record]) -> io::Result<()> {
    if records.len() > MAX_RECORDS || !records.iter().all(valid_record) {
        return Err(invalid_file());
    }
    for record in records {
        if record.name.chars().any(char::is_whitespace)
            || record.values.iter().any(|v| v.is_empty() || v.chars().any(char::is_whitespace))
        {
            return Err(io::Error::new(ErrorKind::InvalidInput, "Текстовый формат не поддерживает пробелы в значениях"));
        }
    }
    let mut text = String::new();
    for record in records {
        text.push(record.kind);
        text.push(' ');
        text.push_str(&record.name);
        for value in &record.values {
            text.push(' ');
            text.push_str(value);
        }
        text.push('\n');
    }
    fs::write(path, text)
}

fn load_text(path: &str) -> io::Result<Vec<Record>> {
    let text = fs::read_to_string(path)?;
    let mut records = Vec::new();
    for line in text.lines() {
        if line.is_empty() { continue; }
        let mut words = line.split_whitespace();
        let (kind, name) = match (words.next(), words.next()) {
            (Some(k), Some(n)) => (k, n),
            _ => return Err(invalid_file()),
        };
        if kind.len() != 1 {
            return Err(invalid_file());
        }
        let values = words.map(str::to_string).collect();
        let record = Record { kind: kind.as_bytes()[0] as char, name: name.to_string(), values };
        if !valid_record(&record) {
            return Err(invalid_file());
        }
        records.push(record);
        if records.len() > MAX_RECORDS {
            return Err(invalid_file());
        }
    }
    Ok(records)
}

pub fn save_records(path: &str, records: &[Record]) -> io::Result<()> {
    if path.ends_with(".bin") {
        save_binary(path, records)
    } else {
        save_text(path, records)
    }
}

pub fn load_records(path: &str) -> io::Result<Vec<Record>> {
    if path.ends_with(".bin") {
        load_binary(path)
    } else {
        load_text(path)
    }
}