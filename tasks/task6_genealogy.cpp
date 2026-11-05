#include <iostream>
#include "io.hpp"
#include "tasks.hpp"
#include "../algorithms/genealogy.hpp"

using namespace std;

bool SolveGenealogy(int n, const Array& pairs, string& out, string& error) {
    FamilyTree tree;
    for (int i = 0; i < pairs.length(); i++) {
        Array w = SplitWords(pairs.at(i));
        if (w.length() != 2) {
            error = "строка \"" + pairs.at(i) + "\": нужно два имени";
            return false;
        }
        if (!tree.add(w.at(0), w.at(1), error)) return false;
    }
    if (tree.size() != n) {
        error = "указано N = " + to_string(n) + ", а в древе " + to_string(tree.size()) + " элементов";
        return false;
    }
    Array report, picture;
    if (!tree.report(report, error) || !tree.draw(picture, error)) return false;
    out = Join(report, "\n") + "\nДрево:\n" + Join(picture, "\n") + "\n";
    return true;
}

void RunGenealogy() {
    cout << "Задание 6.2. Родословная: число потомков. BACK - в меню.\n"
            "Введите N, затем N-1 строк вида \"имя_потомка имя_родителя\"." << endl;
    string line;
    while (Ask("N> ", line)) {
        int n = 0;
        try {
            n = stoi(line);
        } catch (...) {
        }
        if (n < 2) {
            cout << "Ошибка: N - целое число не меньше 2" << endl;
            continue;
        }
        Array pairs;
        for (int i = 1; i < n; i++) {
            string p;
            if (!ReadLine(p)) return;
            pairs.push_back(p);
        }
        string out, error;
        if (SolveGenealogy(n, pairs, out, error)) cout << out;
        else cout << "Ошибка: " << error << endl;
    }
}
