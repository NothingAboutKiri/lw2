#include <iostream>
#include "io.hpp"
#include "tasks.hpp"
#include "../structures/json.hpp"

using namespace std;

static const string HELP =
    "Команды (A, B, C - имена множеств, x - элемент):\n"
    "  SETADD A x        - добавить элемент\n"
    "  SETDEL A x        - удалить элемент\n"
    "  SET_AT A x        - есть ли элемент (TRUE/FALSE)\n"
    "  SET_UNION A B [C] - объединение (с C - сохранить результат в C)\n"
    "  SET_INTER A B [C] - пересечение\n"
    "  SET_DIFF A B [C]  - разность A \\ B\n"
    "  PRINT A | PRINT ALL";

bool SetStore::load(const string& path, string& error) {
    map<string, Array> raw;
    bool exists;
    if (!LoadJson(path, raw, exists)) {
        error = "ошибка в файле " + path;
        return false;
    }
    sets.clear();
    for (const auto& item : raw) {
        HashSet s;
        for (int i = 0; i < item.second.length(); i++) s.add(item.second.at(i));
        sets.emplace(item.first, std::move(s));
    }
    return true;
}

bool SetStore::save(const string& path) const {
    map<string, Array> raw;
    for (const auto& item : sets) raw[item.first] = item.second.sorted();
    return SaveJson(path, raw);
}

string SetStore::execute(const string& query, bool& changed) {
    changed = false;
    Array w = SplitWords(query);
    if (w.length() == 0) return "";
    string cmd = Upper(w.at(0));
    int n = w.length();
    bool ok;
    if (cmd == "SETADD" || cmd == "SETDEL" || cmd == "SET_AT") ok = (n == 3);
    else if (cmd == "SET_UNION" || cmd == "SET_INTER" || cmd == "SET_DIFF") ok = (n == 3 || n == 4);
    else if (cmd == "PRINT") ok = (n == 2);
    else if (cmd == "HELP") ok = (n == 1);
    else return "Ошибка: неизвестная команда (HELP - список команд)";
    if (!ok) return "Ошибка: неверное число аргументов (HELP - список команд)";

    if (cmd == "HELP") return HELP;
    string a = n > 1 ? w.at(1) : "";
    if (cmd == "SETADD") {
        if (!sets[a].add(w.at(2))) return "Элемент уже есть в множестве";
        changed = true;
        return "OK";
    }
    if (cmd == "SETDEL") {
        auto it = sets.find(a);
        if (it == sets.end()) return "Ошибка: множества " + a + " нет";
        if (!it->second.remove(w.at(2))) return "Ошибка: элемента нет в множестве";
        changed = true;
        return "OK";
    }
    if (cmd == "SET_AT") {
        auto it = sets.find(a);
        return (it != sets.end() && it->second.contains(w.at(2))) ? "TRUE" : "FALSE";
    }
    if (cmd == "PRINT") {
        if (Upper(a) == "ALL") {
            if (sets.empty()) return "Множеств нет";
            string out;
            for (const auto& item : sets) {
                if (!out.empty()) out += "\n";
                out += item.first + " = " + item.second.show();
            }
            return out;
        }
        auto it = sets.find(a);
        if (it == sets.end()) return "Ошибка: множества " + a + " нет";
        return a + " = " + it->second.show();
    }
    auto ia = sets.find(a), ib = sets.find(w.at(2));
    if (ia == sets.end()) return "Ошибка: множества " + a + " нет";
    if (ib == sets.end()) return "Ошибка: множества " + w.at(2) + " нет";
    HashSet c = (cmd == "SET_UNION") ? ia->second.unite(ib->second)
              : (cmd == "SET_INTER") ? ia->second.intersect(ib->second)
                                     : ia->second.subtract(ib->second);
    string text = c.show();
    if (n == 4) {
        sets[w.at(3)] = std::move(c);
        changed = true;
        return w.at(3) + " = " + text;
    }
    return text;
}

int RunSetCli(int argc, char* argv[]) {
    string file, query;
    bool hasFile = false, hasQuery = false;
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            file = argv[++i];
            hasFile = true;
        } else if (arg == "--query" && i + 1 < argc) {
            for (int j = i + 1; j < argc; j++) query += (j > i + 1 ? " " : "") + string(argv[j]);
            hasQuery = true;
            break;
        } else {
            break;
        }
    }
    if (!hasFile || !hasQuery) {
        cerr << "Использование: lab2 --file <файл> --query <запрос>\n" << HELP << endl;
        return 1;
    }
    SetStore store;
    string error;
    if (!store.load(file, error)) {
        cerr << "Ошибка: " << error << endl;
        return 1;
    }
    bool changed;
    cout << store.execute(query, changed) << endl;
    if (changed && !store.save(file)) {
        cerr << "Ошибка: не удалось записать файл " << file << endl;
        return 1;
    }
    return 0;
}

void RunSets() {
    const string FILE = "sets.json";
    cout << "Задания 2-3. Множества (файл " << FILE << "). BACK - в меню.\n" << HELP << endl;
    SetStore store;
    string error;
    if (!store.load(FILE, error)) {
        cout << "Предупреждение: " << error << ", начинаем с пустых множеств" << endl;
    }
    string line;
    while (Ask("SET> ", line)) {
        bool changed;
        string out = store.execute(line, changed);
        if (!out.empty()) cout << out << endl;
        if (changed && !store.save(FILE)) cout << "Ошибка: не удалось записать файл " << FILE << endl;
    }
}
