#include <cstdio>
#include <iostream>
#include "io.hpp"
#include "tasks.hpp"
#include "../structures/open_hash.hpp"

using namespace std;

static const string HELP = "Команды: INSERT k v | GET k | DEL k | PRINT | EXPERIMENT [m] | HELP | BACK";

string ExperimentTable(int m) {
    string out = "Эмпирический анализ: m = " + to_string(NextPrime(m)) +
                 ", 20 повторов, среднее число проб (эксперимент / теория)\n"
                 "alpha | линейное: успешный  неуспешный | двойное: успешный  неуспешный\n";
    double alphas[] = {0.25, 0.5, 0.75, 0.9};
    for (double a : alphas) {
        Measure l = Experiment(Method::Linear, m, a, 20), d = Experiment(Method::Double, m, a, 20);
        Measure tl = Theory(Method::Linear, a), td = Theory(Method::Double, a);
        char buf[200];
        snprintf(buf, sizeof(buf), " %.2f | %5.2f / %5.2f  %6.2f / %6.2f | %5.2f / %5.2f  %5.2f / %5.2f\n",
                 a, l.success, tl.success, l.unsuccess, tl.unsuccess, d.success, td.success, d.unsuccess,
                 td.unsuccess);
        out += buf;
    }
    return out;
}

void RunHash() {
    cout << "Задание 6.1. Хеш-таблицы с открытой адресацией.\n" << HELP << endl;
    OpenHash<string> lin(Method::Linear, 11), dbl(Method::Double, 11);
    string line;
    while (Ask("HASH> ", line)) {
        Array w = SplitWords(line);
        if (w.length() == 0) continue;
        string cmd = Upper(w.at(0));
        int n = w.length();
        if (cmd == "INSERT" && n == 3) {
            int a = lin.insert(w.at(1), w.at(2));
            int b = dbl.insert(w.at(1), w.at(2));
            cout << "OK (проб: линейное " << a << ", двойное " << b << ")" << endl;
        } else if (cmd == "GET" && n == 2) {
            string v;
            if (!lin.get(w.at(1), v)) v = "NOT FOUND";
            cout << v << " (проб: линейное " << lin.search_probes(w.at(1)) << ", двойное "
                 << dbl.search_probes(w.at(1)) << ")" << endl;
        } else if (cmd == "DEL" && n == 2) {
            bool ok = lin.remove(w.at(1));
            dbl.remove(w.at(1));
            cout << (ok ? "OK" : "Ошибка: ключа нет") << endl;
        } else if (cmd == "PRINT" && n == 1) {
            const OpenHash<string>* tables[] = {&lin, &dbl};
            const char* titles[] = {"Линейное апробирование", "Двойное хеширование"};
            for (int t = 0; t < 2; t++) {
                char buf[120];
                snprintf(buf, sizeof(buf), "%s: m = %d, n = %d, alpha = %.2f", titles[t], tables[t]->capacity(),
                         tables[t]->size(), tables[t]->load_factor());
                cout << buf << endl;
                Array d = tables[t]->dump();
                for (int i = 0; i < d.length(); i++) {
                    string key = d.at(i).substr(d.at(i).find(": ") + 2), v;
                    cout << "  " << d.at(i) << (tables[t]->get(key, v) ? "=" + v : "") << endl;
                }
            }
        } else if (cmd == "EXPERIMENT" && n <= 2) {
            int m = 1009;
            if (n == 2) {
                try {
                    m = stoi(w.at(1));
                } catch (...) {
                    m = 0;
                }
            }
            if (m < 11) cout << "Ошибка: размер таблицы - целое число не меньше 11" << endl;
            else cout << ExperimentTable(m);
        } else if (cmd == "HELP") {
            cout << HELP << endl;
        } else {
            cout << "Ошибка: неверная команда. " << HELP << endl;
        }
    }
}
