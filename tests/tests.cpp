#include <cstdio>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include "../algorithms/genealogy.hpp"
#include "../algorithms/huffman.hpp"
#include "../algorithms/lru.hpp"
#include "../algorithms/pattern.hpp"
#include "../algorithms/xml.hpp"
#include "../structures/open_hash.hpp"
#include "../tasks/tasks.hpp"

using namespace std;

static int checks = 0;

static void Check(bool condition, const string& description) {
    ++checks;
    if (!condition) throw runtime_error(description);
    cout << "PASS " << setw(3) << checks << "  " << description << '\n';
}

// Feed the same input a user types into the interactive task, then inspect its console output.
template <typename Run>
static string ConsoleOutput(const string& input, Run run) {
    istringstream source(input);
    ostringstream output;
    streambuf* oldInput = cin.rdbuf(source.rdbuf());
    streambuf* oldOutput = cout.rdbuf(output.rdbuf());
    cin.clear();
    run();
    cin.rdbuf(oldInput);
    cout.rdbuf(oldOutput);
    cin.clear();
    return output.str();
}

static void TestXml() {
    Check(IsValidXml(""), "XML: пустая строка допустима");
    Check(IsValidXml("<a><b></b></a><c></c>"), "XML: вложенные и соседние теги");
    Check(!IsValidXml("<a></b>"), "XML: неверный закрывающий тег отклонён");
    Check(!IsValidXml("<A></A>"), "XML: заглавная буква в теге отклонена");
    Check(!IsValidXml("<a>text</a>"), "XML: текст между тегами отклонён");
    string repaired = SolveXml("<a> <ab> </ab> <c> </x> </a>");
    Check(repaired == "<a><ab></ab><c></c></a>", "XML: пример 1 — исправлена одна буква");
    Check(SolveXml("<a> <ab> </ax> </x>") == "Incorrect", "XML: пример 2 — исправление невозможно");
    Check(IsValidXml(repaired), "XML: восстановленная строка корректна");
    Check(SolveXml("<a><xa>") == "<a></a>", "XML: косая черта заменена буквой");
    Check(SolveXml("<a><?a>") == "<a></a>", "XML: косая черта заменена посторонним знаком");
    Check(SolveXml("<a><b><?b></a>") == "<a><b></b></a>",
          "XML: косая черта заменена посторонним знаком во вложенном теге");
    Check(SolveXml("<a>< a>") == "<a></a>", "XML: косая черта заменена пробелом");
    Check(SolveXml("<A></a>") == "<a></a>", "XML: заглавная буква заменена строчной");
    Check(SolveXml("<a>X</a>") == "Incorrect", "XML: текст нельзя исправить одной заменой");
}

static void TestSets() {
    SetStore store;
    bool changed = false;
    Check(store.execute("SETADD A 1", changed) == "OK" && changed, "Множество: добавление элемента");
    Check(store.execute("SETADD A 1", changed) != "OK" && !changed, "Множество: повторный элемент не добавлен");
    store.execute("SETADD A 2", changed);
    store.execute("SETADD B 2", changed);
    store.execute("SETADD B 3", changed);
    Check(store.execute("SET_AT A 1", changed) == "TRUE" && !changed, "Множество: поиск существующего элемента");
    Check(store.execute("SET_AT A 3", changed) == "FALSE", "Множество: поиск отсутствующего элемента");
    Check(store.execute("SET_UNION A B", changed) == "{1, 2, 3}", "Множество: объединение");
    Check(store.execute("SET_INTER A B", changed) == "{2}", "Множество: пересечение");
    Check(store.execute("SET_DIFF A B", changed) == "{1}", "Множество: разность");
    Check(store.execute("SET_UNION A B C", changed) == "C = {1, 2, 3}" && changed,
          "Множество: сохранение результата операции");
    Check(store.execute("SETDEL A 1", changed) == "OK" && changed, "Множество: удаление элемента");
    Check(store.execute("SET_AT A 1", changed) == "FALSE", "Множество: удалённый элемент отсутствует");
    const string path = "lab2_test_sets.json";
    Check(store.save(path), "Множество: сохранение в файл JSON");
    SetStore loaded;
    string error;
    Check(loaded.load(path, error), "Множество: загрузка из файла JSON");
    Check(loaded.execute("PRINT C", changed) == "C = {1, 2, 3}", "Множество: данные сохранились после записи и загрузки");
    std::remove(path.c_str());

    HashSet largeA, largeB;
    for (int i = 0; i < 40; ++i) {
        largeA.add("ключ" + to_string(i));
        largeB.add("ключ" + to_string(i + 20));
    }
    HashSet united = largeA.unite(largeB);
    bool allPresent = united.size() == 60;
    for (int i = 0; i < 60; ++i) allPresent = allPresent && united.contains("ключ" + to_string(i));
    Check(allPresent && largeA.size() == 40 && largeB.size() == 40,
          "Множество: объединение сохраняет элементы после перераспределения по хешу");

    // Проверяем элемент, чей индекс меняется после увеличения таблицы.
    HashSet movedKey;
    movedKey.add("h"); // h: 104 % 8 = 0, после расширения 104 % 16 = 8
    for (int i = 0; i < 6; ++i) movedKey.add("доп" + to_string(i));
    Check(movedKey.size() == 7 && movedKey.contains("h"),
          "Множество: элемент найден по новой ячейке после перехеширования");
    Check(movedKey.remove("h") && !movedKey.contains("h"),
          "Множество: элемент удалён из новой ячейки после перехеширования");

    // Проверяем, что элементы одной старой цепочки не остаются вместе по старому индексу.
    HashSet splitChain;
    splitChain.add("A"); // A и I имеют один индекс при размере 8
    splitChain.add("I"); // при размере 16 их индексы различаются
    for (int i = 0; i < 5; ++i) splitChain.add("элемент" + to_string(i));
    Check(splitChain.size() == 7 && splitChain.contains("A") && splitChain.contains("I"),
          "Множество: элементы одной цепочки найдены после переноса в разные ячейки");
}

static void TestPattern() {
    Check(MatchPattern("meow@stud.nstu.ru", "*@stud.nstu.ru"), "Шаблон: пример 1 — совпадение");
    Check(!MatchPattern("hello", "h?lo"), "Шаблон: пример 2 — несовпадение");
    Check(MatchPattern("sobaka", "s*ak?"), "Шаблон: пример 3 — совпадение");
    Check(MatchPattern("", "***"), "Шаблон: звёздочка заменяет пустую строку");
    Check(!MatchPattern("", "?"), "Шаблон: вопросительный знак требует один символ");
    Check(MatchPattern("abc", "a?c"), "Шаблон: вопросительный знак заменяет один символ");
}

static void TestHuffman() {
    Huffman h;
    Check(!h.build(""), "Хаффман: пустая строка отклонена");
    Check(h.build("abracadabra"), "Хаффман: пример — построение дерева");
    string bits, restored;
    Check(h.encode("abracadabra", bits), "Хаффман: пример — кодирование");
    Check(h.decode(bits, restored) && restored == "abracadabra", "Хаффман: пример — восстановление исходной строки");
    Check(h.weight_of("a") == 5 && h.weight_of("b") == 2, "Хаффман: частоты символов");
    Check(!h.encode("z", bits), "Хаффман: неизвестный символ отклонён");
    Check(!h.decode("2", restored), "Хаффман: неверный бит отклонён");
    Check(h.build("aaaa") && h.encode("aaaa", bits) && bits == "0000", "Хаффман: кодирование строки из одной буквы");
    Check(h.decode(bits, restored) && restored == "aaaa", "Хаффман: декодирование строки из одной буквы");
}

static void TestHash() {
    for (Method method : {Method::Linear, Method::Double}) {
        const string mode = method == Method::Linear ? "линейное пробирование" : "двойное хеширование";
        OpenHash<int> table(method, 3);
        Check(!table.contains("a"), "Хеш-таблица (" + mode + "): отсутствующий ключ не найден");
        for (int i = 0; i < 30; ++i) table.insert("key" + to_string(i), i);
        Check(table.size() == 30 && table.capacity() > 3, "Хеш-таблица (" + mode + "): увеличение размера");
        for (int i = 0; i < 30; ++i) {
            int value = -1;
            Check(table.get("key" + to_string(i), value) && value == i,
                  "Хеш-таблица (" + mode + "): ключ " + to_string(i) + " найден после увеличения");
        }
        table.insert("key10", 100);
        int value = -1;
        Check(table.size() == 30 && table.get("key10", value) && value == 100,
              "Хеш-таблица (" + mode + "): обновление значения");
        Check(table.remove("key10") && !table.contains("key10"),
              "Хеш-таблица (" + mode + "): удаление ключа");
        Check(!table.remove("key10"), "Хеш-таблица (" + mode + "): повторное удаление отклонено");
        table.insert("key10", 10);
        Check(table.get("key10", value) && value == 10,
              "Хеш-таблица (" + mode + "): вставка после удаления");
    }
    Check(NextPrime(10) == 11, "Хеш-таблица: размер округляется до простого числа");
}

static void TestGenealogy() {
    Array pairs;
    for (const char* pair : {"Alexei Peter_I", "Anna Peter_I", "Elizabeth Peter_I",
                               "Peter_II Alexei", "Peter_III Anna", "Paul_I Peter_III",
                               "Alexander_I Paul_I", "Nicholaus_I Paul_I"}) {
        pairs.push_back(pair);
    }
    string out, error;
    Check(SolveGenealogy(9, pairs, out, error), "Родословная: пример — древо построено");
    Check(out.find("Alexander_I 0\nAlexei 1\nAnna 4\nElizabeth 0\nNicholaus_I 0\n"
                   "Paul_I 2\nPeter_I 8\nPeter_II 0\nPeter_III 3\n") == 0,
          "Родословная: пример — число потомков указано верно");
    Check(out.find("Древо:\nPeter_I\n") != string::npos, "Родословная: древо выведено");
    pairs = Array();
    pairs.push_back("A B");
    pairs.push_back("B A");
    Check(!SolveGenealogy(2, pairs, out, error), "Родословная: цикл отклонён");
    pairs = Array();
    pairs.push_back("A B");
    pairs.push_back("A C");
    Check(!SolveGenealogy(3, pairs, out, error), "Родословная: два родителя отклонены");
}

static void TestLru() {
    LruCache cache(2);
    string value;
    cache.set("1", "2");
    Check(cache.get("1", value) && value == "2", "Кэш LRU: пример 1 — получение значения");
    Array operations;
    for (const char* token : {"SET", "1", "2", "SET", "2", "3", "SET", "1", "5",
                                "SET", "4", "5", "SET", "6", "7", "GET", "4",
                                "SET", "1", "2", "GET", "3"}) operations.push_back(token);
    string out, state, error;
    Check(SolveLru(2, operations, out, state, error), "Кэш LRU: пример 2 — запросы обработаны");
    Check(out == "5 -1" && state == "4->5, 1->2", "Кэш LRU: пример 2 — ответы и порядок элементов верны");
    Check(!SolveLru(2, Array(1, "GET"), out, state, error), "Кэш LRU: запрос без ключа отклонён");
    LruCache zero(0);
    zero.set("x", "y");
    Check(!zero.get("x", value), "Кэш LRU: нулевая ёмкость");
}

static void TestConsole() {
    string out = ConsoleOutput("<a> <ab> </ab> <c> </x> </a>\nBACK\n", RunXml);
    Check(out.find("<a><ab></ab><c></c></a>") != string::npos,
          "Консоль XML: исправленная строка выведена");
    out = ConsoleOutput("<a><?a>\nBACK\n", RunXml);
    Check(out.find("<a></a>") != string::npos,
          "Консоль XML: восстановлена заменённая косая черта");
    out = ConsoleOutput("<a><b><?b></a>\nBACK\n", RunXml);
    Check(out.find("<a><b></b></a>") != string::npos,
          "Консоль XML: восстановлена косая черта во вложенном теге");

    out = ConsoleOutput("hello\nh?lo\nBACK\n", RunPattern);
    Check(out.find("не соответствует") != string::npos,
          "Консоль шаблона: несовпадение выведено");

    out = ConsoleOutput("2 2\nSET 1 2\nGET 1\nBACK\n", RunLru);
    Check(out.find("Вывод: 2") != string::npos,
          "Консоль кэша LRU: ответ из примера выведен");
    out = ConsoleOutput("2 2\nSET 1 10\nGET 1\n2 1\nGET 1\nBACK\n", RunLru);
    Check(out.find("Вывод: 10") != string::npos && out.find("Вывод: -1") != string::npos,
          "Консоль кэша LRU: новая серия запросов начинается с пустого кэша");
    out = ConsoleOutput("2 8\nSET 1 2, SET 2 3, SET 1 5, SET 4 5, "
                        "SET 6 7, GET 4, SET 1 2, GET 3\nBACK\n", RunLru);
    Check(out.find("Вывод: 5 -1\nКэш (от старых к новым): 4->5, 1->2") != string::npos,
          "Консоль кэша LRU: запросы через запятую дают правильный результат");

    out = ConsoleOutput("INSERT a 7\nGET a\nBACK\n", RunHash);
    Check(out.find("7 (проб:") != string::npos,
          "Консоль хеш-таблицы: добавленное значение выведено");

    out = ConsoleOutput("abracadabra\nBACK\n", RunHuffman);
    Check(out.find("Декодирование: abracadabra") != string::npos,
          "Консоль алгоритма Хаффмана: декодированный текст выведен");

    out = ConsoleOutput("2\nChild Parent\nBACK\n", RunGenealogy);
    Check(out.find("Parent 1") != string::npos && out.find("Child 0") != string::npos,
          "Консоль родословной: число потомков выведено");

    const string path = "lab2_test_console_sets.json";
    SetStore store;
    bool changed;
    store.execute("SETADD A 42", changed);
    Check(store.save(path), "Консоль множества: входной файл подготовлен");
    int exitCode = -1;
    out = ConsoleOutput("", [&] {
        string arg0 = "lab2", arg1 = "--file", arg2 = path;
        string arg3 = "--query", arg4 = "SET_AT", arg5 = "A", arg6 = "42";
        char* args[] = {arg0.data(), arg1.data(), arg2.data(), arg3.data(),
                        arg4.data(), arg5.data(), arg6.data()};
        exitCode = RunSetCli(7, args);
    });
    Check(exitCode == 0, "Консоль множества: команда завершилась успешно");
    Check(out == "TRUE\n", "Консоль множества: результат поиска выведен");
    std::remove(path.c_str());
}

int main() {
    try {
        TestXml();
        TestSets();
        TestPattern();
        TestHuffman();
        TestHash();
        TestGenealogy();
        TestLru();
        TestConsole();
        cout << "ИТОГО: " << checks << " проверок пройдено\n";
        return 0;
    } catch (const exception& e) {
        cerr << "ОШИБКА на проверке " << checks << ": " << e.what() << '\n';
        return 1;
    }
}
