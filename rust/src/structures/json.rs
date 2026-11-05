use crate::structures::array::Array;
use std::collections::BTreeMap;
use std::fs;

pub fn json_string(text: &str) -> String {
    let mut result = String::from("\"");
    for c in text.chars() {
        if c == '"' || c == '\\' {
            result.push('\\');
            result.push(c);
        } else if (c as u32) < 0x20 {
            result.push_str(&format!("\\u{:04x}", c as u32));
        } else {
            result.push(c);
        }
    }
    result.push('"');
    result
}

pub fn save(path: &str, items: &BTreeMap<String, Array<String>>) -> bool {
    let mut text = String::from("{\n");
    let mut first = true;
    for (name, values) in items {
        if !first {
            text.push_str(",\n");
        }
        first = false;
        text.push_str(&format!("  {}: [", json_string(name)));
        for (i, value) in values.as_slice().iter().enumerate() {
            if i > 0 {
                text.push_str(", ");
            }
            text.push_str(&json_string(value));
        }
        text.push(']');
    }
    if !first {
        text.push('\n');
    }
    text.push_str("}\n");
    fs::write(path, text).is_ok()
}

struct Parser {
    chars: Vec<char>,
    pos: usize,
}

impl Parser {
    fn skip_spaces(&mut self) {
        while self.pos < self.chars.len() && matches!(self.chars[self.pos], ' ' | '\t' | '\n' | '\r') {
            self.pos += 1;
        }
    }

    fn expect(&mut self, c: char) -> bool {
        self.skip_spaces();
        if self.pos < self.chars.len() && self.chars[self.pos] == c {
            self.pos += 1;
            return true;
        }
        false
    }

    fn next(&mut self) -> Option<char> {
        let c = *self.chars.get(self.pos)?;
        self.pos += 1;
        Some(c)
    }

    fn read_string(&mut self) -> Option<String> {
        if !self.expect('"') {
            return None;
        }
        let mut out = String::new();
        loop {
            let c = self.next()?;
            if c == '"' {
                return Some(out);
            }
            if c != '\\' {
                out.push(c);
                continue;
            }
            match self.next()? {
                '"' => out.push('"'),
                '\\' => out.push('\\'),
                '/' => out.push('/'),
                'n' => out.push('\n'),
                't' => out.push('\t'),
                'r' => out.push('\r'),
                'b' => out.push('\u{8}'),
                'f' => out.push('\u{c}'),
                'u' => {
                    let mut code = 0u32;
                    for _ in 0..4 {
                        code = code * 16 + self.next()?.to_digit(16)?;
                    }
                    out.push(char::from_u32(code)?);
                }
                _ => return None,
            }
        }
    }

    fn read_value(&mut self) -> Option<String> {
        self.skip_spaces();
        if self.pos < self.chars.len() && self.chars[self.pos] == '"' {
            return self.read_string();
        }
        let mut out = String::new();
        while self.pos < self.chars.len() && !matches!(self.chars[self.pos], ',' | ']' | ' ' | '\n' | '\r' | '\t') {
            out.push(self.chars[self.pos]);
            self.pos += 1;
        }
        if out.is_empty() {
            None
        } else {
            Some(out)
        }
    }

    fn parse(&mut self) -> Option<BTreeMap<String, Array<String>>> {
        let mut result = BTreeMap::new();
        if !self.expect('{') {
            return None;
        }
        if !self.expect('}') {
            loop {
                let name = self.read_string()?;
                if !self.expect(':') || !self.expect('[') {
                    return None;
                }
                let mut values = Array::new();
                if !self.expect(']') {
                    loop {
                        let value = self.read_value()?;
                        values.push_back(value);
                        if self.expect(']') {
                            break;
                        }
                        if !self.expect(',') {
                            return None;
                        }
                    }
                }
                result.insert(name, values);
                if self.expect('}') {
                    break;
                }
                if !self.expect(',') {
                    return None;
                }
            }
        }
        self.skip_spaces();
        if self.pos != self.chars.len() {
            return None;
        }
        Some(result)
    }
}

pub fn load(path: &str) -> Result<BTreeMap<String, Array<String>>, String> {
    let text = match fs::read_to_string(path) {
        Ok(text) => text,
        Err(_) => return Ok(BTreeMap::new()),
    };
    let mut parser = Parser { chars: text.chars().collect(), pos: 0 };
    parser.parse().ok_or(format!("ошибка в файле {}", path))
}
