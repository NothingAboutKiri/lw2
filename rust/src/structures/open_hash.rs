// Задание 6.1. Хеш-таблица с открытой адресацией.
// Методы разрешения коллизий: линейное апробирование и двойное хеширование.
// Ключи, значения и состояния ячеек хранятся в трёх массивах Array из ЛР №1.
use crate::structures::array::Array;
use crate::structures::hash_set::hash_str;

#[derive(Clone, Copy, PartialEq, Debug)]
pub enum Method {
    Linear, // (h1 + i) mod m
    Double, // (h1 + i * h2) mod m, h2 = 1 + H mod (m - 1)
}

const EMPTY: u8 = 0;
const BUSY: u8 = 1;
const DELETED: u8 = 2; // «надгробие»: поиск через такую ячейку продолжается

pub struct OpenHash {
    method: Method,
    keys: Array<String>,
    values: Array<String>,
    state: Array<u8>,
    count: usize,   // занятые ячейки
    deleted: usize, // ячейки-надгробия
    pub auto_resize: bool,
}

pub fn is_prime(n: usize) -> bool {
    if n < 2 {
        return false;
    }
    let mut d = 2;
    while d * d <= n {
        if n % d == 0 {
            return false;
        }
        d += 1;
    }
    true
}

pub fn next_prime(mut n: usize) -> usize {
    while !is_prime(n) {
        n += 1;
    }
    n
}

impl OpenHash {
    // size округляется вверх до простого числа
    pub fn new(method: Method, size: usize) -> OpenHash {
        let m = next_prime(size.max(3));
        OpenHash {
            method,
            keys: Array::filled(m, String::new()),
            values: Array::filled(m, String::new()),
            state: Array::filled(m, EMPTY),
            count: 0,
            deleted: 0,
            auto_resize: true,
        }
    }

    pub fn capacity(&self) -> usize {
        self.state.length()
    }

    pub fn len(&self) -> usize {
        self.count
    }

    pub fn load_factor(&self) -> f64 {
        self.count as f64 / self.capacity() as f64
    }

    // i-я проба для ключа
    fn probe(&self, h: u64, i: usize) -> usize {
        let m = self.capacity() as u64;
        let h1 = h % m;
        let step = match self.method {
            Method::Linear => 1,
            Method::Double => 1 + h % (m - 1),
        };
        ((h1 + (i as u64) * step) % m) as usize
    }

    // Поиск ячейки ключа: (номер ячейки или None, число проб)
    fn find(&self, key: &str) -> (Option<usize>, usize) {
        let h = hash_str(key);
        for i in 0..self.capacity() {
            let pos = self.probe(h, i);
            match *self.state.at(pos) {
                EMPTY => return (None, i + 1),
                BUSY if self.keys.at(pos) == key => return (Some(pos), i + 1),
                _ => {}
            }
        }
        (None, self.capacity())
    }

    // Перестроить таблицу размером new_size, все ключи вставляются заново
    fn rebuild(&mut self, new_size: usize) {
        let mut t = OpenHash::new(self.method, new_size);
        t.auto_resize = false;
        for i in 0..self.capacity() {
            if *self.state.at(i) == BUSY {
                t.insert(self.keys.at(i), self.values.at(i));
            }
        }
        t.auto_resize = self.auto_resize;
        *self = t;
    }

    // Вставка или замена значения; возвращает число проб
    pub fn insert(&mut self, key: &str, value: &str) -> usize {
        if self.auto_resize && (self.count + self.deleted + 1) * 4 > self.capacity() * 3 {
            // заполнено больше 75 %: увеличить вдвое (или просто очистить надгробия)
            let size = if (self.count + 1) * 2 > self.capacity() { self.capacity() * 2 } else { self.capacity() };
            self.rebuild(size);
        }
        let (found, probes) = self.find(key);
        if let Some(pos) = found {
            self.values.set(pos, value.to_string());
            return probes;
        }
        let h = hash_str(key);
        for i in 0..self.capacity() {
            let pos = self.probe(h, i);
            let s = *self.state.at(pos);
            if s != BUSY {
                if s == DELETED {
                    self.deleted -= 1;
                }
                self.keys.set(pos, key.to_string());
                self.values.set(pos, value.to_string());
                self.state.set(pos, BUSY);
                self.count += 1;
                return i + 1;
            }
        }
        panic!("хеш-таблица переполнена");
    }

    pub fn get(&self, key: &str) -> Option<String> {
        self.find(key).0.map(|pos| self.values.at(pos).clone())
    }

    pub fn contains(&self, key: &str) -> bool {
        self.find(key).0.is_some()
    }

    // Число проб при поиске (для эксперимента)
    pub fn search_probes(&self, key: &str) -> usize {
        self.find(key).1
    }

    pub fn remove(&mut self, key: &str) -> bool {
        match self.find(key).0 {
            Some(pos) => {
                self.state.set(pos, DELETED);
                self.keys.set(pos, String::new());
                self.values.set(pos, String::new());
                self.count -= 1;
                self.deleted += 1;
                true
            }
            None => false,
        }
    }

    // Содержимое ячеек для вывода: "3: key=value", пустые пропускаются, надгробия - "3: <deleted>"
    pub fn dump(&self) -> Array<String> {
        let mut result = Array::new();
        for i in 0..self.capacity() {
            match *self.state.at(i) {
                BUSY => result.push_back(format!("{}: {}={}", i, self.keys.at(i), self.values.at(i))),
                DELETED => result.push_back(format!("{}: <deleted>", i)),
                _ => {}
            }
        }
        result
    }
}

// ---------- Эмпирический анализ ----------

// Простой генератор псевдослучайных чисел (xorshift), чтобы опыт повторялся
pub struct Rng(u64);

impl Rng {
    pub fn new(seed: u64) -> Rng {
        Rng(seed.wrapping_mul(6364136223846793005).wrapping_add(1442695040888963407) | 1)
    }
    pub fn next(&mut self) -> u64 {
        let mut x = self.0;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        self.0 = x;
        x
    }
}

pub struct Measure {
    pub success: f64,   // среднее число проб при успешном поиске
    pub unsuccess: f64, // при неуспешном
}

// Заполнить таблицу размером m до коэффициента alpha и измерить среднее число проб.
// Опыт повторяется runs раз, результаты усредняются.
pub fn experiment(method: Method, m: usize, alpha: f64, runs: u64) -> Measure {
    let mut succ_total = 0.0;
    let mut uns_total = 0.0;
    for run in 0..runs {
        let mut rng = Rng::new(run + 1);
        let mut t = OpenHash::new(method, m);
        t.auto_resize = false;
        let n = (alpha * t.capacity() as f64) as usize;
        let mut keys: Array<String> = Array::new();
        while t.len() < n {
            let k = format!("k{}", rng.next() % 1_000_000_000);
            if !t.contains(&k) {
                t.insert(&k, "1");
                keys.push_back(k);
            }
        }
        let mut s = 0usize;
        for k in keys.as_slice() {
            s += t.search_probes(k);
        }
        succ_total += s as f64 / n as f64;
        let queries = 2000;
        let mut u = 0usize;
        let mut q = 0;
        while q < queries {
            let k = format!("u{}", rng.next() % 1_000_000_000);
            u += t.search_probes(&k);
            q += 1;
        }
        uns_total += u as f64 / queries as f64;
    }
    Measure { success: succ_total / runs as f64, unsuccess: uns_total / runs as f64 }
}

// Теоретические оценки Д. Кнута
pub fn theory(method: Method, a: f64) -> Measure {
    match method {
        Method::Linear => Measure {
            success: 0.5 * (1.0 + 1.0 / (1.0 - a)),
            unsuccess: 0.5 * (1.0 + 1.0 / ((1.0 - a) * (1.0 - a))),
        },
        Method::Double => Measure { success: (1.0 / a) * (1.0 / (1.0 - a)).ln(), unsuccess: 1.0 / (1.0 - a) },
    }
}
