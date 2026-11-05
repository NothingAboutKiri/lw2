// Задания 2 и 3. Множество строк на хеш-таблице с методом цепочек.
// Таблица - массив Array из ЛР №1, в каждой ячейке - односвязный список (цепочка).
use crate::structures::array::Array;
use crate::structures::singly_list::SinglyList;

// Полиномиальный хеш строки: h = h * 31 + код символа (mod 2^64)
pub fn hash_str(text: &str) -> u64 {
    let mut h: u64 = 0;
    for b in text.bytes() {
        h = h.wrapping_mul(31).wrapping_add(b as u64);
    }
    h
}

const START_BUCKETS: usize = 8;
const MAX_LOAD: f64 = 0.75; // при большем заполнении таблица увеличивается вдвое

pub struct HashSet {
    buckets: Array<SinglyList>,
    count: usize,
}

impl HashSet {
    pub fn new() -> HashSet {
        HashSet { buckets: Array::filled(START_BUCKETS, SinglyList::new()), count: 0 }
    }

    fn index(&self, x: &str) -> usize {
        (hash_str(x) % self.buckets.length() as u64) as usize
    }

    // Перехеширование: новый массив ячеек в два раза больше, элементы распределяются заново
    fn rehash(&mut self) {
        let new_size = self.buckets.length() * 2;
        let old = std::mem::replace(&mut self.buckets, Array::filled(new_size, SinglyList::new()));
        for chain in old.as_slice() {
            for x in chain.values() {
                let i = self.index(&x);
                self.buckets.at_mut(i).push_front(&x);
            }
        }
    }

    // SETADD: false, если элемент уже есть
    pub fn add(&mut self, x: &str) -> bool {
        if self.contains(x) {
            return false;
        }
        if (self.count + 1) as f64 > MAX_LOAD * self.buckets.length() as f64 {
            self.rehash();
        }
        let i = self.index(x);
        self.buckets.at_mut(i).push_front(x);
        self.count += 1;
        true
    }

    // SETDEL: false, если элемента нет
    pub fn remove(&mut self, x: &str) -> bool {
        let i = self.index(x);
        if self.buckets.at_mut(i).remove(x) {
            self.count -= 1;
            true
        } else {
            false
        }
    }

    // SET_AT
    pub fn contains(&self, x: &str) -> bool {
        self.buckets.at(self.index(x)).contains(x)
    }

    pub fn len(&self) -> usize {
        self.count
    }

    // Все элементы (обход всех цепочек), O(n + m)
    pub fn values(&self) -> Array<String> {
        let mut result = Array::new();
        for chain in self.buckets.as_slice() {
            for x in chain.values() {
                result.push_back(x);
            }
        }
        result
    }

    // ---------- Задание 3: дополнительные операции ----------

    // Объединение A ∪ B, O(|A| + |B|)
    pub fn union(&self, other: &HashSet) -> HashSet {
        let mut c = HashSet::new();
        for x in self.values().as_slice() {
            c.add(x);
        }
        for x in other.values().as_slice() {
            c.add(x);
        }
        c
    }

    // Пересечение A ∩ B: перебирается меньшее множество, O(min(|A|, |B|))
    pub fn intersection(&self, other: &HashSet) -> HashSet {
        let (small, big) = if self.len() <= other.len() { (self, other) } else { (other, self) };
        let mut c = HashSet::new();
        for x in small.values().as_slice() {
            if big.contains(x) {
                c.add(x);
            }
        }
        c
    }

    // Разность A \ B, O(|A|)
    pub fn difference(&self, other: &HashSet) -> HashSet {
        let mut c = HashSet::new();
        for x in self.values().as_slice() {
            if !other.contains(x) {
                c.add(x);
            }
        }
        c
    }

    // Элементы по возрастанию - для вывода (числа сравниваются как числа)
    pub fn sorted(&self) -> Array<String> {
        let mut keys: Array<SortKey> = Array::new();
        for x in self.values().as_slice() {
            keys.push_back(SortKey(x.clone()));
        }
        let sorted = crate::structures::array::merge_sort(&keys);
        let mut result = Array::new();
        for k in sorted.as_slice() {
            result.push_back(k.0.clone());
        }
        result
    }

    // {1, 2, 3}
    pub fn show(&self) -> String {
        format!("{{{}}}", self.sorted().as_slice().join(", "))
    }
}

impl Default for HashSet {
    fn default() -> Self {
        HashSet::new()
    }
}

// Ключ сортировки: целые числа - численно и раньше слов, слова - по алфавиту
#[derive(Clone, Default, PartialEq)]
struct SortKey(String);

impl PartialOrd for SortKey {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        match (self.0.parse::<i64>(), other.0.parse::<i64>()) {
            (Ok(a), Ok(b)) => a.partial_cmp(&b),
            (Ok(_), Err(_)) => Some(std::cmp::Ordering::Less),
            (Err(_), Ok(_)) => Some(std::cmp::Ordering::Greater),
            _ => self.0.partial_cmp(&other.0),
        }
    }
}
