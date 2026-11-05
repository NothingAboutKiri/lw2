use crate::structures::singly_list::SinglyList;

pub struct Stack {
    list: SinglyList,
}

impl Stack {
    pub fn new() -> Stack {
        Stack { list: SinglyList::new() }
    }

    pub fn push(&mut self, value: &str) {
        self.list.push_front(value);
    }

    pub fn pop(&mut self) -> Option<String> {
        self.list.pop_front()
    }

    pub fn is_empty(&self) -> bool {
        self.list.is_empty()
    }
}

impl Default for Stack {
    fn default() -> Self {
        Stack::new()
    }
}
