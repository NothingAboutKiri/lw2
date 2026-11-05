use super::io::{ask, split_words};
use crate::algorithms::huffman::Huffman;

pub fn table(h: &Huffman) -> String {
    let mut out = String::from("Символ | Частота | Код\n");
    for c in h.symbols.as_slice() {
        let shown = if *c == ' ' { "' '".to_string() } else { c.to_string() };
        out.push_str(&format!("{:>6} | {:>7} | {}\n", shown, h.weight_of(*c), h.code_of(*c).unwrap()));
    }
    out
}

pub fn run() {
    println!("Задание 5. Алгоритм Хаффмана.");
    println!("Введите строку для кодирования; DECODE <биты> - декодировать по последнему дереву; BACK - в меню.");
    let mut last: Option<Huffman> = None;
    while let Some(line) = ask("HUF> ") {
        let words = split_words(&line);
        if words.len() == 2 && words[0].eq_ignore_ascii_case("DECODE") {
            match &last {
                None => println!("Ошибка: сначала закодируйте строку"),
                Some(h) => match h.decode(&words[1]) {
                    Some(s) => println!("Строка: {}", s),
                    None => println!("Ошибка: код не соответствует дереву"),
                },
            }
            continue;
        }
        let h = match Huffman::build(&line) {
            Some(h) => h,
            None => {
                println!("Ошибка: пустая строка");
                continue;
            }
        };
        let code = h.encode(&line).unwrap();
        print!("{}", table(&h));
        println!("Код: {}", code);
        println!("Длина: {} бит (без сжатия {} бит)", code.len(), line.chars().count() * 8);
        println!("Декодирование: {}", h.decode(&code).unwrap());
        last = Some(h);
    }
}
