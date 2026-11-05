use crate::structures::array::Array;
use crate::structures::open_hash::{Method, OpenHash};
use crate::structures::singly_list::SinglyList;
use crate::structures::stack::Stack;

#[derive(Clone, Default)]
pub struct HNode {
    pub symbol: String,
    pub weight: usize,
    pub left: Option<usize>,
    pub right: Option<usize>,
}

pub struct Huffman {
    pub nodes: Array<HNode>,
    pub root: usize,
    pub symbols: Array<char>,
    pub codes: OpenHash,
}

impl Huffman {
    pub fn build(text: &str) -> Option<Huffman> {
        if text.is_empty() {
            return None;
        }
        let mut freq = OpenHash::new(Method::Double, 16);
        let mut symbols: Array<char> = Array::new();
        for c in text.chars() {
            let key = c.to_string();
            match freq.get(&key) {
                Some(v) => {
                    freq.insert(&key, &(v.parse::<usize>().unwrap() + 1).to_string());
                }
                None => {
                    freq.insert(&key, "1");
                    symbols.push_back(c);
                }
            }
        }
        let mut nodes: Array<HNode> = Array::new();
        let mut free = SinglyList::new();
        for c in symbols.as_slice() {
            let w = freq.get(&c.to_string()).unwrap().parse().unwrap();
            nodes.push_back(HNode { symbol: c.to_string(), weight: w, left: None, right: None });
            insert_sorted(&mut free, &nodes, nodes.length() - 1);
        }
        while free.length() > 1 {
            let a: usize = free.pop_front().unwrap().parse().unwrap();
            let b: usize = free.pop_front().unwrap().parse().unwrap();
            let parent = HNode {
                symbol: format!("{}{}", nodes.at(a).symbol, nodes.at(b).symbol),
                weight: nodes.at(a).weight + nodes.at(b).weight,
                left: Some(a),
                right: Some(b),
            };
            nodes.push_back(parent);
            insert_sorted(&mut free, &nodes, nodes.length() - 1);
        }
        let root: usize = free.pop_front().unwrap().parse().unwrap();

        let mut codes = OpenHash::new(Method::Double, symbols.length() * 2);
        let mut stack = Stack::new();
        stack.push(&format!("{} ", root));
        while let Some(item) = stack.pop() {
            let (idx, code) = item.split_once(' ').unwrap();
            let node = nodes.at(idx.parse().unwrap()).clone();
            match (node.left, node.right) {
                (Some(l), Some(r)) => {
                    stack.push(&format!("{} {}1", r, code));
                    stack.push(&format!("{} {}0", l, code));
                }
                _ => {
                    let code = if code.is_empty() { "0" } else { code };
                    codes.insert(&node.symbol, code);
                }
            }
        }
        Some(Huffman { nodes, root, symbols, codes })
    }

    pub fn code_of(&self, c: char) -> Option<String> {
        self.codes.get(&c.to_string())
    }

    pub fn encode(&self, text: &str) -> Option<String> {
        let mut out = String::new();
        for c in text.chars() {
            out.push_str(&self.code_of(c)?);
        }
        Some(out)
    }

    pub fn decode(&self, bits: &str) -> Option<String> {
        let root = self.nodes.at(self.root);
        if root.left.is_none() {
            if bits.chars().all(|b| b == '0') {
                return Some(root.symbol.repeat(bits.len()));
            }
            return None;
        }
        let mut out = String::new();
        let mut cur = self.root;
        for b in bits.chars() {
            let node = self.nodes.at(cur);
            cur = match b {
                '0' => node.left?,
                '1' => node.right?,
                _ => return None,
            };
            let next = self.nodes.at(cur);
            if next.left.is_none() {
                out.push_str(&next.symbol);
                cur = self.root;
            }
        }
        if cur != self.root {
            return None;
        }
        Some(out)
    }

    pub fn weight_of(&self, c: char) -> usize {
        for n in self.nodes.as_slice() {
            if n.left.is_none() && n.symbol == c.to_string() {
                return n.weight;
            }
        }
        0
    }
}

fn insert_sorted(free: &mut SinglyList, nodes: &Array<HNode>, idx: usize) {
    let w = nodes.at(idx).weight;
    for v in free.values() {
        let other: usize = v.parse().unwrap();
        if nodes.at(other).weight > w {
            free.insert_before(&v, &idx.to_string());
            return;
        }
    }
    free.push_back(&idx.to_string());
}
