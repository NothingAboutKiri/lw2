use super::io::{ask, read_line, split_words};
use crate::algorithms::lru::Lru;

pub fn solve(cap: usize, queries: &[String]) -> Result<(String, String), String> {
    let mut cache = Lru::new(cap);
    let mut out = Vec::new();
    let mut i = 0;
    while i < queries.len() {
        match queries[i].to_ascii_uppercase().as_str() {
            "SET" if i + 2 < queries.len() => {
                cache.set(&queries[i + 1], &queries[i + 2]);
                i += 3;
            }
            "GET" if i + 1 < queries.len() => {
                out.push(cache.get(&queries[i + 1]).unwrap_or("-1".to_string()));
                i += 2;
            }
            _ => return Err(format!("неверный запрос около \"{}\"", queries[i])),
        }
    }
    Ok((out.join(" "), cache.state().as_slice().join(", ")))
}

fn numbers(line: &str) -> Vec<usize> {
    line.split(|c: char| !c.is_ascii_digit()).filter(|s| !s.is_empty()).filter_map(|s| s.parse().ok()).collect()
}

pub fn run() {
    println!("Задание 7. LRU-кэш. BACK - в меню.");
    println!("Первая строка: cap Q; далее Q запросов SET x y / GET x (можно в одну строку).");
    while let Some(line) = ask("cap Q> ") {
        let nums = numbers(&line);
        if nums.len() != 2 {
            println!("Ошибка: нужно два числа: ёмкость и количество запросов");
            continue;
        }
        let (cap, q) = (nums[0], nums[1]);
        let mut tokens: Vec<String> = Vec::new();
        let mut count = 0;
        while count < q {
            let l = match read_line() {
                Some(l) => l,
                None => return,
            };
            for w in split_words(&l) {
                if w.eq_ignore_ascii_case("Queries") || w == "=" {
                    continue;
                }
                if w.eq_ignore_ascii_case("SET") || w.eq_ignore_ascii_case("GET") {
                    count += 1;
                }
                tokens.push(w);
            }
        }
        match solve(cap, &tokens) {
            Ok((out, state)) => {
                println!("Вывод: {}", out);
                println!("Кэш (от старых к новым): {}", state);
            }
            Err(e) => println!("Ошибка: {}", e),
        }
    }
}
