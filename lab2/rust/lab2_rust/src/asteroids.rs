#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Asteroid {
    pub mass: u64,
    pub direction: char,
}

/// Разбирает строку: числа задают массы, R/L — направления астероидов.
pub fn parse(text: &str) -> Result<Vec<Asteroid>, String> {
    let bytes = text.as_bytes();
    let mut masses = Vec::<u64>::new();
    let mut directions = Vec::<char>::new();
    let mut i = 0usize;

    while i < bytes.len() {
        let byte = bytes[i];
        if byte.is_ascii_digit() {
            let mut mass = 0u64;
            while i < bytes.len() && bytes[i].is_ascii_digit() {
                let digit = u64::from(bytes[i] - b'0');
                mass = mass
                    .checked_mul(10)
                    .and_then(|v| v.checked_add(digit))
                    .ok_or_else(|| "Ошибка: Размер астероида слишком большой".to_string())?;
                i += 1;
            }
            masses.push(mass);
            continue;
        }
        match byte {
            b'R' | b'r' => directions.push('R'),
            b'L' | b'l' => directions.push('L'),
            _ => {}
        }
        i += 1;
    }

    if masses.len() != directions.len() {
        return Err(
            "Ошибка: У каждого астероида должны быть размер и направление (R или L)".to_string(),
        );
    }

    Ok(masses
        .into_iter()
        .zip(directions)
        .map(|(mass, direction)| Asteroid { mass, direction })
        .collect())
}

/// Возвращает стек выживших астероидов (верх стека находится в конце Vec).
pub fn resolve(input: &[Asteroid]) -> Vec<Asteroid> {
    let mut output = Vec::<Asteroid>::new();
    for asteroid in input.iter().copied() {
        let mut current = asteroid;
        let mut alive = true;

        while current.direction == 'L'
            && output.last().map_or(false, |top| top.direction == 'R')
        {
            let top = output.pop().expect("последний элемент уже проверен");
            if top.mass < current.mass {
                current.mass -= top.mass;
            } else if top.mass == current.mass {
                alive = false;
                break;
            } else {
                output.push(Asteroid {
                    mass: top.mass - current.mass,
                    direction: 'R',
                });
                alive = false;
                break;
            }
        }

        if alive {
            output.push(current);
        }
    }
    output
}

pub fn print_result(stack: &[Asteroid]) {
    print!("[");
    for (index, asteroid) in stack.iter().enumerate() {
        if index > 0 {
            print!(", ");
        }
        print!("('{}', '{}')", asteroid.mass, asteroid.direction);
    }
    println!("]");
}
