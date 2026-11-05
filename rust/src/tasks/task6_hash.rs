use super::io::{ask, split_words};
use crate::structures::open_hash::{experiment, theory, Method, OpenHash};

const HELP: &str = "Команды: INSERT k v | GET k | DEL k | PRINT | EXPERIMENT [m] | HELP | BACK";

pub fn experiment_table(m: usize) -> String {
    let mut out = format!(
        "Эмпирический анализ: m = {}, 20 повторов, среднее число проб (эксперимент / теория)\n\
         alpha | линейное: успешный  неуспешный | двойное: успешный  неуспешный\n",
        crate::structures::open_hash::next_prime(m)
    );
    for a in [0.25, 0.5, 0.75, 0.9] {
        let l = experiment(Method::Linear, m, a, 20);
        let d = experiment(Method::Double, m, a, 20);
        let tl = theory(Method::Linear, a);
        let td = theory(Method::Double, a);
        out.push_str(&format!(
            " {:.2} | {:5.2} / {:5.2}  {:6.2} / {:6.2} | {:5.2} / {:5.2}  {:5.2} / {:5.2}\n",
            a, l.success, tl.success, l.unsuccess, tl.unsuccess, d.success, td.success, d.unsuccess, td.unsuccess
        ));
    }
    out
}

pub fn run() {
    println!("Задание 6.1. Хеш-таблицы с открытой адресацией.\n{}", HELP);
    let mut lin = OpenHash::new(Method::Linear, 11);
    let mut dbl = OpenHash::new(Method::Double, 11);
    while let Some(line) = ask("HASH> ") {
        let w = split_words(&line);
        if w.is_empty() {
            continue;
        }
        match (w[0].to_ascii_uppercase().as_str(), w.len()) {
            ("INSERT", 3) => {
                let a = lin.insert(&w[1], &w[2]);
                let b = dbl.insert(&w[1], &w[2]);
                println!("OK (проб: линейное {}, двойное {})", a, b);
            }
            ("GET", 2) => {
                let v = lin.get(&w[1]);
                println!("{} (проб: линейное {}, двойное {})", v.unwrap_or("NOT FOUND".to_string()),
                    lin.search_probes(&w[1]), dbl.search_probes(&w[1]));
            }
            ("DEL", 2) => {
                let ok = lin.remove(&w[1]);
                dbl.remove(&w[1]);
                println!("{}", if ok { "OK" } else { "Ошибка: ключа нет" });
            }
            ("PRINT", 1) => {
                for (name, t) in [("Линейное апробирование", &lin), ("Двойное хеширование", &dbl)] {
                    println!("{}: m = {}, n = {}, alpha = {:.2}", name, t.capacity(), t.len(), t.load_factor());
                    for s in t.dump().as_slice() {
                        println!("  {}", s);
                    }
                }
            }
            ("EXPERIMENT", 1) => print!("{}", experiment_table(1009)),
            ("EXPERIMENT", 2) => match w[1].parse::<usize>() {
                Ok(m) if m >= 11 => print!("{}", experiment_table(m)),
                _ => println!("Ошибка: размер таблицы - целое число не меньше 11"),
            },
            ("HELP", 1) => println!("{}", HELP),
            _ => println!("Ошибка: неверная команда. {}", HELP),
        }
    }
}
