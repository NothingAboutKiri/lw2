#include <iostream>
#include "io.hpp"
#include "tasks.hpp"
#include "../algorithms/pattern.hpp"

using namespace std;

static string Unquote(const string& s) {
    return s == "\"\"" ? "" : s;
}

string SolvePattern(const string& text, const string& pattern) {
    string t = Unquote(text), p = Unquote(pattern);
    return "Последовательность \"" + t + (MatchPattern(t, p) ? "\" соответствует" : "\" не соответствует") +
           " шаблону \"" + p + "\"";
}

void RunPattern() {
    cout << "Задание 4. Сопоставление с шаблоном: ? - один символ, * - любая последовательность.\n"
            "Пустая строка вводится как \"\". BACK - в меню." << endl;
    string text, pattern;
    while (Ask("Строка> ", text) && Ask("Шаблон> ", pattern)) {
        cout << SolvePattern(text, pattern) << endl;
    }
}
