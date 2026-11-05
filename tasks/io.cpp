#include <iostream>
#include "io.hpp"

using namespace std;

bool ReadLine(string& line) {
    if (!getline(cin, line)) return false;
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
    return true;
}

string Upper(string s) {
    for (char& c : s) {
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    }
    return s;
}

bool Ask(const string& prompt, string& line) {
    cout << prompt << flush;
    if (!ReadLine(line)) return false;
    Array w = SplitWords(line);
    return !(w.length() == 1 && Upper(w.at(0)) == "BACK");
}

Array SplitWords(const string& line) {
    Array words;
    string current;
    for (char c : line) {
        if (c == ' ' || c == '\t') {
            if (!current.empty()) words.push_back(current);
            current = "";
        } else {
            current += c;
        }
    }
    if (!current.empty()) words.push_back(current);
    return words;
}

string Join(const Array& items, const string& sep) {
    string out;
    for (int i = 0; i < items.length(); i++) {
        if (i > 0) out += sep;
        out += items.at(i);
    }
    return out;
}
