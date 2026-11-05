use std::io::{self, Write};

pub fn read_line() -> Option<String> {
    let mut line = String::new();
    if io::stdin().read_line(&mut line).unwrap_or(0) == 0 {
        return None;
    }
    while line.ends_with('\n') || line.ends_with('\r') {
        line.pop();
    }
    Some(line)
}

pub fn prompt(text: &str) {
    print!("{}", text);
    io::stdout().flush().unwrap();
}

pub fn ask(text: &str) -> Option<String> {
    prompt(text);
    let line = read_line()?;
    if line.trim().eq_ignore_ascii_case("BACK") {
        return None;
    }
    Some(line)
}

pub fn split_words(line: &str) -> Vec<String> {
    line.split_whitespace().map(|w| w.to_string()).collect()
}
