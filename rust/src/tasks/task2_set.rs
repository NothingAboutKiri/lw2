use super::io::{ask, split_words};
use crate::structures::hash_set::HashSet;
use crate::structures::json;
use std::collections::BTreeMap;

pub const HELP: &str = "Команды (A, B, C - имена множеств, x - элемент):\n\
  SETADD A x       - добавить элемент\n\
  SETDEL A x       - удалить элемент\n\
  SET_AT A x       - проверить, есть ли элемент (TRUE/FALSE)\n\
  SET_UNION A B [C] - объединение (с C - сохранить результат в C)\n\
  SET_INTER A B [C] - пересечение\n\
  SET_DIFF A B [C]  - разность A \\ B\n\
  PRINT A | PRINT ALL";

pub struct Sets {
    pub items: BTreeMap<String, HashSet>,
}

impl Sets {
    pub fn load(path: &str) -> Result<Sets, String> {
        let raw = json::load(path)?;
        let mut items = BTreeMap::new();
        for (name, values) in raw {
            let mut s = HashSet::new();
            for v in values.as_slice() {
                s.add(v);
            }
            items.insert(name, s);
        }
        Ok(Sets { items })
    }

    pub fn save(&self, path: &str) -> bool {
        let mut raw = BTreeMap::new();
        for (name, s) in &self.items {
            raw.insert(name.clone(), s.sorted());
        }
        json::save(path, &raw)
    }

    fn get(&self, name: &str) -> Result<&HashSet, String> {
        self.items.get(name).ok_or(format!("Ошибка: множества {} нет", name))
    }

    pub fn execute(&mut self, query: &str) -> (String, bool) {
        let w = split_words(query);
        if w.is_empty() {
            return (String::new(), false);
        }
        let cmd = w[0].to_ascii_uppercase();
        let ok = match cmd.as_str() {
            "SETADD" | "SETDEL" | "SET_AT" => w.len() == 3,
            "SET_UNION" | "SET_INTER" | "SET_DIFF" => w.len() == 3 || w.len() == 4,
            "PRINT" => w.len() == 2,
            "HELP" => w.len() == 1,
            _ => return ("Ошибка: неизвестная команда (HELP - список команд)".to_string(), false),
        };
        if !ok {
            return ("Ошибка: неверное число аргументов (HELP - список команд)".to_string(), false);
        }
        match cmd.as_str() {
            "HELP" => (HELP.to_string(), false),
            "SETADD" => {
                let s = self.items.entry(w[1].clone()).or_default();
                if s.add(&w[2]) {
                    ("OK".to_string(), true)
                } else {
                    ("Элемент уже есть в множестве".to_string(), false)
                }
            }
            "SETDEL" => match self.items.get_mut(&w[1]) {
                Some(s) => {
                    if s.remove(&w[2]) {
                        ("OK".to_string(), true)
                    } else {
                        ("Ошибка: элемента нет в множестве".to_string(), false)
                    }
                }
                None => (format!("Ошибка: множества {} нет", w[1]), false),
            },
            "SET_AT" => match self.items.get(&w[1]) {
                Some(s) => ((if s.contains(&w[2]) { "TRUE" } else { "FALSE" }).to_string(), false),
                None => ("FALSE".to_string(), false),
            },
            "PRINT" => {
                if w[1].eq_ignore_ascii_case("ALL") {
                    if self.items.is_empty() {
                        return ("Множеств нет".to_string(), false);
                    }
                    let lines: Vec<String> = self.items.iter().map(|(n, s)| format!("{} = {}", n, s.show())).collect();
                    return (lines.join("\n"), false);
                }
                match self.get(&w[1]) {
                    Ok(s) => (format!("{} = {}", w[1], s.show()), false),
                    Err(e) => (e, false),
                }
            }
            _ => {
                let a = match self.get(&w[1]) {
                    Ok(s) => s,
                    Err(e) => return (e, false),
                };
                let b = match self.get(&w[2]) {
                    Ok(s) => s,
                    Err(e) => return (e, false),
                };
                let c = match cmd.as_str() {
                    "SET_UNION" => a.union(b),
                    "SET_INTER" => a.intersection(b),
                    _ => a.difference(b),
                };
                let text = c.show();
                if w.len() == 4 {
                    self.items.insert(w[3].clone(), c);
                    return (format!("{} = {}", w[3], text), true);
                }
                (text, false)
            }
        }
    }
}

pub fn run_cli(args: &[String]) -> i32 {
    let mut file = None;
    let mut query = None;
    let mut i = 0;
    while i < args.len() {
        match args[i].as_str() {
            "--file" if i + 1 < args.len() => {
                file = Some(args[i + 1].clone());
                i += 2;
            }
            "--query" if i + 1 < args.len() => {
                query = Some(args[i + 1..].join(" "));
                break;
            }
            _ => {
                eprintln!("Использование: lab2 --file <файл> --query <запрос>\n{}", HELP);
                return 1;
            }
        }
    }
    let (file, query) = match (file, query) {
        (Some(f), Some(q)) => (f, q),
        _ => {
            eprintln!("Использование: lab2 --file <файл> --query <запрос>");
            return 1;
        }
    };
    let mut sets = match Sets::load(&file) {
        Ok(s) => s,
        Err(e) => {
            eprintln!("Ошибка: {}", e);
            return 1;
        }
    };
    let (out, changed) = sets.execute(&query);
    println!("{}", out);
    if changed && !sets.save(&file) {
        eprintln!("Ошибка: не удалось записать файл {}", file);
        return 1;
    }
    0
}

pub fn run() {
    const FILE: &str = "sets.json";
    println!("Задания 2-3. Множества (файл {}). BACK - в меню.\n{}", FILE, HELP);
    let mut sets = match Sets::load(FILE) {
        Ok(s) => s,
        Err(e) => {
            println!("Предупреждение: {}, начинаем с пустых множеств", e);
            Sets { items: BTreeMap::new() }
        }
    };
    while let Some(line) = ask("SET> ") {
        let (out, changed) = sets.execute(&line);
        if !out.is_empty() {
            println!("{}", out);
        }
        if changed && !sets.save(FILE) {
            println!("Ошибка: не удалось записать файл {}", FILE);
        }
    }
}
