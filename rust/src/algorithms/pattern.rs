use crate::structures::array::Array;

pub fn matches(text: &str, pattern: &str) -> bool {
    let mut s: Array<char> = Array::new();
    for c in text.chars() {
        s.push_back(c);
    }
    let mut p: Array<char> = Array::new();
    for c in pattern.chars() {
        p.push_back(c);
    }
    let (n, m) = (s.length(), p.length());

    let mut prev: Array<bool> = Array::filled(m + 1, false);
    prev.set(0, true);
    for j in 1..=m {
        let v = *prev.at(j - 1) && *p.at(j - 1) == '*';
        prev.set(j, v);
    }
    for i in 1..=n {
        let mut cur: Array<bool> = Array::filled(m + 1, false);
        for j in 1..=m {
            let pc = *p.at(j - 1);
            let v = if pc == '*' {
                *prev.at(j) || *cur.at(j - 1)
            } else if pc == '?' || pc == *s.at(i - 1) {
                *prev.at(j - 1)
            } else {
                false
            };
            cur.set(j, v);
        }
        prev = cur;
    }
    *prev.at(m)
}
