#[derive(Debug, Clone)]
pub struct Arr {
    data: Vec<Vec<u8>>,
}

impl Default for Arr {
    fn default() -> Self {
        Self::new()
    }
}

impl Arr {
    pub fn new() -> Self {
        Self { data: Vec::with_capacity(2) }
    }

    pub fn insert_tail(&mut self, value: String) {
        self.data.push(value.into_bytes());
    }

    pub fn insert_byte(&mut self, byte: u8) {
        self.data.push(vec![byte]);
    }

    pub fn search(&self, index: isize) -> Option<&[u8]> {
        if index < 0 {
            println!("Ошибка: Такого индекса нет");
            return None;
        }
        match self.data.get(index as usize) {
            Some(value) => Some(value.as_slice()),
            None => {
                println!("Ошибка: Такого индекса нет");
                None
            }
        }
    }

    pub fn histogram(&self) {
        let mut counts = [0usize; 128];
        for item in &self.data {
            let Some(&byte) = item.first() else {
                continue;
            };
            if matches!(byte, b' ' | b'\n' | b'\r' | b'\t') {
                continue;
            }
            if byte >= 128 {
                println!("Ошибка: Символ вне ASCII не поддерживается");
                return;
            }
            counts[byte as usize] += 1;
        }

        let mut symbols = String::new();
        let mut heights = Vec::new();
        let mut max_height = 0usize;
        for (code, count) in counts.iter().enumerate() {
            if *count > 0 {
                symbols.push(char::from(code as u8));
                heights.push(*count);
                max_height = max_height.max(*count);
            }
        }
        if heights.is_empty() {
            println!("Ошибка: Нет символов для гистограммы");
            return;
        }

        for level in (1..=max_height).rev() {
            let mut line = String::with_capacity(heights.len());
            for height in &heights {
                line.push(if *height >= level { '#' } else { ' ' });
            }
            println!("{}", line.trim_end_matches(' '));
        }
        println!("{}", symbols);
    }
}
