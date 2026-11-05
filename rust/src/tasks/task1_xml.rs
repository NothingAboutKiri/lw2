use super::io::ask;
use crate::algorithms::xml::{is_valid_str, restore};

pub fn solve(line: &str) -> String {
    let s: String = line.chars().filter(|c| !c.is_whitespace()).collect();
    if s.chars().any(|c| !(c.is_ascii_lowercase() || c == '<' || c == '>' || c == '/')) {
        return "Ошибка: допустимы только строчные латинские буквы и символы < > /".to_string();
    }
    if is_valid_str(&s) {
        return format!("{} (строка уже корректна)", s);
    }
    match restore(&s) {
        Some(fixed) => fixed,
        None => "Incorrect".to_string(),
    }
}

pub fn run() {
    println!("Задание 1. Восстановление XML-строки (стек). BACK - в меню.");
    while let Some(line) = ask("XML> ") {
        if line.trim().is_empty() {
            continue;
        }
        let len = line.chars().filter(|c| !c.is_whitespace()).count();
        if !(7..=1000).contains(&len) {
            println!("Предупреждение: по условию длина строки от 7 до 1000 (сейчас {})", len);
        }
        println!("{}", solve(&line));
    }
}
