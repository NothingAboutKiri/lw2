use super::io::{ask, read_line, split_words};
use crate::algorithms::genealogy::Tree;

pub fn solve(n: usize, pairs: &[String]) -> Result<String, String> {
    let mut tree = Tree::new();
    for line in pairs {
        let w = split_words(line);
        if w.len() != 2 {
            return Err(format!("строка \"{}\": нужно два имени", line));
        }
        tree.add(&w[0], &w[1])?;
    }
    if tree.len() != n {
        return Err(format!("указано N = {}, а в древе {} элементов", n, tree.len()));
    }
    let mut out = String::new();
    for s in tree.report()?.as_slice() {
        out.push_str(s);
        out.push('\n');
    }
    out.push_str("Древо:\n");
    for s in tree.draw()?.as_slice() {
        out.push_str(s);
        out.push('\n');
    }
    Ok(out)
}

pub fn run() {
    println!("Задание 6.2. Родословная: число потомков. BACK - в меню.");
    println!("Введите N, затем N-1 строк вида \"имя_потомка имя_родителя\".");
    while let Some(line) = ask("N> ") {
        let n: usize = match line.trim().parse() {
            Ok(n) if n >= 1 => n,
            _ => {
                println!("Ошибка: N - целое число не меньше 1");
                continue;
            }
        };
        let mut pairs = Vec::new();
        for _ in 1..n {
            match read_line() {
                Some(l) => pairs.push(l),
                None => return,
            }
        }
        if n == 1 {
            println!("Ошибка: при N = 1 имя родоначальника неизвестно - нужно хотя бы две строки");
            continue;
        }
        match solve(n, &pairs) {
            Ok(text) => print!("{}", text),
            Err(e) => println!("Ошибка: {}", e),
        }
    }
}
