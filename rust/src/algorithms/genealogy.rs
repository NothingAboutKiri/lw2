use crate::structures::array::{merge_sort, Array};
use crate::structures::open_hash::{Method, OpenHash};
use crate::structures::singly_list::SinglyList;
use crate::structures::stack::Stack;

pub struct Tree {
    index: OpenHash,
    names: Array<String>,
    parent: Array<Option<usize>>,
    children: Array<SinglyList>,
}

impl Tree {
    pub fn new() -> Tree {
        Tree {
            index: OpenHash::new(Method::Double, 16),
            names: Array::new(),
            parent: Array::new(),
            children: Array::new(),
        }
    }

    fn id(&mut self, name: &str) -> usize {
        if let Some(i) = self.index.get(name) {
            return i.parse().unwrap();
        }
        let i = self.names.length();
        self.index.insert(name, &i.to_string());
        self.names.push_back(name.to_string());
        self.parent.push_back(None);
        self.children.push_back(SinglyList::new());
        i
    }

    pub fn add(&mut self, child: &str, parent: &str) -> Result<(), String> {
        if child == parent {
            return Err(format!("{} не может быть своим родителем", child));
        }
        let c = self.id(child);
        let p = self.id(parent);
        if self.parent.at(c).is_some() {
            return Err(format!("у {} уже есть родитель", child));
        }
        self.parent.set(c, Some(p));
        self.children.at_mut(p).push_back(child);
        Ok(())
    }

    pub fn len(&self) -> usize {
        self.names.length()
    }

    fn root(&self) -> Result<usize, String> {
        let mut root = None;
        for i in 0..self.len() {
            if self.parent.at(i).is_none() {
                if root.is_some() {
                    return Err("у древа больше одного родоначальника".to_string());
                }
                root = Some(i);
            }
        }
        root.ok_or("нет родоначальника (связи образуют цикл)".to_string())
    }

    fn sorted_children(&self, i: usize) -> Array<String> {
        let mut a = Array::new();
        for name in self.children.at(i).values() {
            a.push_back(name);
        }
        merge_sort(&a)
    }

    pub fn descendants(&self) -> Result<Array<usize>, String> {
        let root = self.root()?;
        let mut order: Array<usize> = Array::new();
        let mut stack = Stack::new();
        stack.push(&root.to_string());
        while let Some(top) = stack.pop() {
            let v: usize = top.parse().unwrap();
            order.push_back(v);
            for name in self.children.at(v).values() {
                stack.push(&self.index.get(&name).unwrap());
            }
        }
        if order.length() != self.len() {
            return Err("древо несвязно (есть цикл)".to_string());
        }
        let mut count: Array<usize> = Array::filled(self.len(), 0);
        for k in (0..order.length()).rev() {
            let v = *order.at(k);
            let mut s = 0;
            for name in self.children.at(v).values() {
                let c: usize = self.index.get(&name).unwrap().parse().unwrap();
                s += 1 + *count.at(c);
            }
            count.set(v, s);
        }
        Ok(count)
    }

    pub fn report(&self) -> Result<Array<String>, String> {
        let count = self.descendants()?;
        let sorted = merge_sort(&self.names);
        let mut out = Array::new();
        for name in sorted.as_slice() {
            let i: usize = self.index.get(name).unwrap().parse().unwrap();
            out.push_back(format!("{} {}", name, count.at(i)));
        }
        Ok(out)
    }

    pub fn draw(&self) -> Result<Array<String>, String> {
        let root = self.root()?;
        let mut out = Array::new();
        let mut stack = Stack::new();
        stack.push(&format!("0 {}", self.names.at(root)));
        while let Some(top) = stack.pop() {
            let (depth, name) = top.split_once(' ').unwrap();
            let depth: usize = depth.parse().unwrap();
            out.push_back(format!("{}{}", "  ".repeat(depth), name));
            let i: usize = self.index.get(name).unwrap().parse().unwrap();
            let kids = self.sorted_children(i);
            for k in (0..kids.length()).rev() {
                stack.push(&format!("{} {}", depth + 1, kids.at(k)));
            }
        }
        Ok(out)
    }
}

impl Default for Tree {
    fn default() -> Self {
        Tree::new()
    }
}
