struct Node {
    value: String,
    prev: Option<usize>,
    next: Option<usize>,
}

pub struct DoublyList {
    nodes: Vec<Option<Node>>,
    free: Vec<usize>,
    head: Option<usize>,
    tail: Option<usize>,
    count: usize,
}

impl DoublyList {
    pub fn new() -> DoublyList {
        DoublyList { nodes: Vec::new(), free: Vec::new(), head: None, tail: None, count: 0 }
    }

    fn node_mut(&mut self, i: usize) -> &mut Node {
        self.nodes[i].as_mut().unwrap()
    }

    pub fn push_front(&mut self, value: &str) -> usize {
        let node = Node { value: value.to_string(), prev: None, next: self.head };
        let i = match self.free.pop() {
            Some(i) => {
                self.nodes[i] = Some(node);
                i
            }
            None => {
                self.nodes.push(Some(node));
                self.nodes.len() - 1
            }
        };
        match self.head {
            Some(h) => self.node_mut(h).prev = Some(i),
            None => self.tail = Some(i),
        }
        self.head = Some(i);
        self.count += 1;
        i
    }

    pub fn remove(&mut self, i: usize) -> String {
        let node = self.nodes[i].take().unwrap();
        match node.prev {
            Some(p) => self.node_mut(p).next = node.next,
            None => self.head = node.next,
        }
        match node.next {
            Some(n) => self.node_mut(n).prev = node.prev,
            None => self.tail = node.prev,
        }
        self.free.push(i);
        self.count -= 1;
        node.value
    }

    pub fn back(&self) -> Option<usize> {
        self.tail
    }

    pub fn length(&self) -> usize {
        self.count
    }

    pub fn values(&self) -> Vec<String> {
        let mut result = Vec::new();
        let mut current = self.head;
        while let Some(i) = current {
            let node = self.nodes[i].as_ref().unwrap();
            result.push(node.value.clone());
            current = node.next;
        }
        result
    }
}
