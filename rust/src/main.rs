mod algorithms;
mod structures;
mod tasks;

use tasks::io::ask;

const MENU: &str = "\n\
+-----+----------------------------------------------+\n\
| No  | Задание                                      |\n\
+-----+----------------------------------------------+\n\
| 1   | Стек: восстановление XML-строки              |\n\
| 2   | Множество: SETADD, SETDEL, SET_AT,           |\n\
|     | объединение, пересечение, разность (зад. 3)  |\n\
| 4   | Массив: сопоставление с шаблоном             |\n\
| 5   | Дерево: алгоритм Хаффмана                    |\n\
| 6.1 | Хеш-таблица: открытая адресация, анализ      |\n\
| 6.2 | Хеш-таблица: родословная                     |\n\
| 7   | LRU-кэш                                      |\n\
| 0   | Выход                                        |\n\
+-----+----------------------------------------------+";

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    if !args.is_empty() {
        std::process::exit(tasks::task2_set::run_cli(&args));
    }
    loop {
        println!("{}", MENU);
        let choice = match ask("Выбор: ") {
            Some(c) => c,
            None => return,
        };
        match choice.trim() {
            "0" => return,
            "1" => tasks::task1_xml::run(),
            "2" | "3" => tasks::task2_set::run(),
            "4" => tasks::task4_pattern::run(),
            "5" => tasks::task5_huffman::run(),
            "6.1" | "6" => tasks::task6_hash::run(),
            "6.2" => tasks::task6_genealogy::run(),
            "7" => tasks::task7_lru::run(),
            _ => println!("Ошибка: такого пункта нет"),
        }
    }
}
