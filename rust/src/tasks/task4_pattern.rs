use super::io::ask;
use crate::algorithms::pattern::matches;

fn unquote(s: &str) -> &str {
    if s == "\"\"" {
        ""
    } else {
        s
    }
}

pub fn solve(text: &str, pattern: &str) -> String {
    let (t, p) = (unquote(text), unquote(pattern));
    if matches(t, p) {
        format!("Последовательность \"{}\" соответствует шаблону \"{}\"", t, p)
    } else {
        format!("Последовательность \"{}\" не соответствует шаблону \"{}\"", t, p)
    }
}

pub fn run() {
    println!("Задание 4. Сопоставление с шаблоном: ? - один символ, * - любая последовательность.");
    println!("Пустая строка вводится как \"\". BACK - в меню.");
    loop {
        let text = match ask("Строка> ") {
            Some(t) => t,
            None => return,
        };
        let pattern = match ask("Шаблон> ") {
            Some(p) => p,
            None => return,
        };
        println!("{}", solve(&text, &pattern));
    }
}
