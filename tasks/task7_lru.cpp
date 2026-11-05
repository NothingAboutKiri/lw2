#include <iostream>
#include "io.hpp"
#include "tasks.hpp"
#include "../algorithms/lru.hpp"

using namespace std;

bool SolveLru(int cap, const Array& t, string& out, string& state, string& error) {
    LruCache cache(cap);
    Array results;
    int i = 0;
    while (i < t.length()) {
        string cmd = Upper(t.at(i));
        if (cmd == "SET" && i + 2 < t.length()) {
            cache.set(t.at(i + 1), t.at(i + 2));
            i += 3;
        } else if (cmd == "GET" && i + 1 < t.length()) {
            string v;
            results.push_back(cache.get(t.at(i + 1), v) ? v : "-1");
            i += 2;
        } else {
            error = "неверный запрос около \"" + t.at(i) + "\"";
            return false;
        }
    }
    out = Join(results, " ");
    state = Join(cache.state(), ", ");
    return true;
}

static DynArray<int> Numbers(const string& line) {
    DynArray<int> nums;
    string cur;
    for (char c : line + " ") {
        if (c >= '0' && c <= '9') {
            cur += c;
        } else if (!cur.empty()) {
            nums.push_back(stoi(cur));
            cur = "";
        }
    }
    return nums;
}

void RunLru() {
    cout << "Задание 7. LRU-кэш. BACK - в меню.\n"
            "Первая строка: cap Q; далее Q запросов SET x y / GET x (можно в одну строку)." << endl;
    string line;
    while (Ask("cap Q> ", line)) {
        DynArray<int> nums = Numbers(line);
        if (nums.length() != 2) {
            cout << "Ошибка: нужно два числа: ёмкость и количество запросов" << endl;
            continue;
        }
        int cap = nums.at(0), q = nums.at(1), count = 0;
        Array tokens;
        while (count < q) {
            string l;
            if (!ReadLine(l)) return;
            // Запросы можно разделять запятыми, как в записи примеров задания.
            for (char& c : l) {
                if (c == ',') c = ' ';
            }
            Array w = SplitWords(l);
            for (int i = 0; i < w.length(); i++) {
                string u = Upper(w.at(i));
                if (u == "QUERIES" || u == "=") continue;
                if (u == "SET" || u == "GET") count++;
                tokens.push_back(w.at(i));
            }
        }
        string out, state, error;
        if (SolveLru(cap, tokens, out, state, error)) {
            cout << "Вывод: " << out << "\nКэш (от старых к новым): " << state << endl;
        } else {
            cout << "Ошибка: " << error << endl;
        }
    }
}
