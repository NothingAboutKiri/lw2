#include <iostream>
#include "io.hpp"
#include "tasks.hpp"
#include "../algorithms/xml.hpp"

using namespace std;

// Spaces between tags are allowed for readable input; spaces inside a tag may be
// the damaged character, so they must reach the one-character repair algorithm.
static string NormalizeXmlInput(const string& line) {
    string s;
    for (size_t i = 0; i < line.size();) {
        if (line[i] == ' ' || line[i] == '\t') {
            size_t next = i;
            while (next < line.size() && (line[next] == ' ' || line[next] == '\t')) next++;
            if (!s.empty() && s.back() == '>' && next < line.size() && line[next] == '<') {
                i = next;
                continue;
            }
        }
        s += line[i++];
    }
    return s;
}

string SolveXml(const string& line) {
    string s = NormalizeXmlInput(line);
    if (IsValidXml(s)) return s + " (строка уже корректна)";
    string fixed;
    return RestoreXml(s, fixed) ? fixed : "Incorrect";
}

void RunXml() {
    cout << "Задание 1. Восстановление XML-строки (стек). BACK - в меню." << endl;
    string line;
    while (Ask("XML> ", line)) {
        if (SplitWords(line).length() == 0) continue;
        int len = static_cast<int>(NormalizeXmlInput(line).size());
        if (len < 7 || len > 1000) {
            cout << "Предупреждение: по условию длина строки от 7 до 1000 (сейчас " << len << ")" << endl;
        }
        cout << SolveXml(line) << endl;
    }
}
