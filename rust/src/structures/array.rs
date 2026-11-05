pub struct Array<T> {
    data: Box<[T]>,
    size: usize,
}

fn new_buffer<T: Default>(n: usize) -> Box<[T]> {
    (0..n).map(|_| T::default()).collect()
}

impl<T: Default + Clone> Array<T> {
    pub fn new() -> Array<T> {
        Array { data: new_buffer(4), size: 0 }
    }

    pub fn filled(n: usize, value: T) -> Array<T> {
        let mut a = Array { data: new_buffer(n.max(1)), size: 0 };
        for _ in 0..n {
            a.push_back(value.clone());
        }
        a
    }

    fn resize(&mut self) {
        let mut new_data = new_buffer(self.data.len() * 2);
        for i in 0..self.size {
            new_data[i] = std::mem::take(&mut self.data[i]);
        }
        self.data = new_data;
    }

    pub fn push_back(&mut self, value: T) {
        if self.size == self.data.len() {
            self.resize();
        }
        self.data[self.size] = value;
        self.size += 1;
    }

    pub fn at(&self, index: usize) -> &T {
        &self.as_slice()[index]
    }

    pub fn at_mut(&mut self, index: usize) -> &mut T {
        let size = self.size;
        &mut self.data[..size][index]
    }

    pub fn set(&mut self, index: usize, value: T) -> bool {
        if index >= self.size {
            return false;
        }
        self.data[index] = value;
        true
    }

    pub fn length(&self) -> usize {
        self.size
    }

    pub fn as_slice(&self) -> &[T] {
        &self.data[..self.size]
    }
}

impl<T: Default + Clone> Default for Array<T> {
    fn default() -> Self {
        Array::new()
    }
}

impl<T: Default + Clone> Clone for Array<T> {
    fn clone(&self) -> Self {
        let mut a = Array::new();
        for x in self.as_slice() {
            a.push_back(x.clone());
        }
        a
    }
}

pub fn merge_sort<T: Default + Clone + PartialOrd>(a: &Array<T>) -> Array<T> {
    let n = a.length();
    if n <= 1 {
        return a.clone();
    }
    let mut left = Array::new();
    let mut right = Array::new();
    for i in 0..n {
        if i < n / 2 {
            left.push_back(a.at(i).clone());
        } else {
            right.push_back(a.at(i).clone());
        }
    }
    let left = merge_sort(&left);
    let right = merge_sort(&right);
    let mut result = Array::new();
    let (mut i, mut j) = (0, 0);
    while i < left.length() && j < right.length() {
        if left.at(i) <= right.at(j) {
            result.push_back(left.at(i).clone());
            i += 1;
        } else {
            result.push_back(right.at(j).clone());
            j += 1;
        }
    }
    while i < left.length() {
        result.push_back(left.at(i).clone());
        i += 1;
    }
    while j < right.length() {
        result.push_back(right.at(j).clone());
        j += 1;
    }
    result
}
