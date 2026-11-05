use crate::structures::array::Array;
use crate::structures::doubly_list::DoublyList;
use crate::structures::open_hash::{Method, OpenHash};

pub struct Lru {
    cap: usize,
    order: DoublyList,
    node: OpenHash,
    value: OpenHash,
}

impl Lru {
    pub fn new(cap: usize) -> Lru {
        Lru {
            cap,
            order: DoublyList::new(),
            node: OpenHash::new(Method::Double, cap * 2 + 3),
            value: OpenHash::new(Method::Double, cap * 2 + 3),
        }
    }

    fn touch(&mut self, key: &str) {
        let id: usize = self.node.get(key).unwrap().parse().unwrap();
        self.order.remove(id);
        let new_id = self.order.push_front(key);
        self.node.insert(key, &new_id.to_string());
    }

    pub fn get(&mut self, key: &str) -> Option<String> {
        let v = self.value.get(key)?;
        self.touch(key);
        Some(v)
    }

    pub fn set(&mut self, key: &str, val: &str) {
        if self.cap == 0 {
            return;
        }
        if self.value.contains(key) {
            self.value.insert(key, val);
            self.touch(key);
            return;
        }
        if self.order.length() == self.cap {
            let tail = self.order.back().unwrap();
            let old = self.order.remove(tail);
            self.node.remove(&old);
            self.value.remove(&old);
        }
        let id = self.order.push_front(key);
        self.node.insert(key, &id.to_string());
        self.value.insert(key, val);
    }

    pub fn state(&self) -> Array<String> {
        let keys = self.order.values();
        let mut out = Array::new();
        for k in keys.iter().rev() {
            out.push_back(format!("{}->{}", k, self.value.get(k).unwrap()));
        }
        out
    }
}
