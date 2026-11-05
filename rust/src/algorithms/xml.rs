use crate::structures::array::Array;
use crate::structures::stack::Stack;

fn is_name(chars: &Array<char>, from: usize, to: usize) -> bool {
    from < to && (from..to).all(|i| chars.at(i).is_ascii_lowercase())
}

pub fn is_valid(chars: &Array<char>) -> bool {
    let n = chars.length();
    let mut stack = Stack::new();
    let mut i = 0;
    while i < n {
        if *chars.at(i) != '<' {
            return false;
        }
        let mut j = i + 1;
        while j < n && *chars.at(j) != '>' {
            j += 1;
        }
        if j == n {
            return false;
        }
        if j > i + 1 && *chars.at(i + 1) == '/' {
            if !is_name(chars, i + 2, j) {
                return false;
            }
            let name: String = (i + 2..j).map(|k| *chars.at(k)).collect();
            match stack.pop() {
                Some(top) if top == name => {}
                _ => return false,
            }
        } else {
            if !is_name(chars, i + 1, j) {
                return false;
            }
            let name: String = (i + 1..j).map(|k| *chars.at(k)).collect();
            stack.push(&name);
        }
        i = j + 1;
    }
    stack.is_empty()
}

pub fn is_valid_str(s: &str) -> bool {
    is_valid(&to_array(s))
}

pub fn to_array(s: &str) -> Array<char> {
    let mut a = Array::new();
    for c in s.chars() {
        a.push_back(c);
    }
    a
}

const ALPHABET: &str = "abcdefghijklmnopqrstuvwxyz<>/";

pub fn restore(s: &str) -> Option<String> {
    let mut chars = to_array(s);
    let n = chars.length();
    for i in (0..n).rev() {
        let original = *chars.at(i);
        for c in ALPHABET.chars() {
            if c == original {
                continue;
            }
            chars.set(i, c);
            if is_valid(&chars) {
                return Some(chars.as_slice().iter().collect());
            }
        }
        chars.set(i, original);
    }
    None
}
