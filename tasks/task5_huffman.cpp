#include <iostream>
#include "io.hpp"
#include "tasks.hpp"
#include "../algorithms/huffman.hpp"

using namespace std;

void RunHuffman() {
    cout << "Задание 5. Алгоритм Хаффмана.\n"
            "Введите строку для кодирования; DECODE <биты> - декодировать по последнему дереву; BACK - в меню."
         << endl;
    Huffman h;
    bool built = false;
    string line;
    while (Ask("HUF> ", line)) {
        Array w = SplitWords(line);
        if (w.length() == 2 && Upper(w.at(0)) == "DECODE") {
            string text;
            if (!built) cout << "Ошибка: сначала закодируйте строку" << endl;
            else if (h.decode(w.at(1), text)) cout << "Строка: " << text << endl;
            else cout << "Ошибка: код не соответствует дереву" << endl;
            continue;
        }
        if (!h.build(line)) {
            cout << "Ошибка: пустая строка" << endl;
            continue;
        }
        built = true;
        cout << "Символ | Частота | Код" << endl;
        Array t = h.table();
        for (int i = 0; i < t.length(); i++) {
            Array parts = SplitWords(t.at(i));
            int n = parts.length();
            string symbol = parts.at(0);
            for (int k = 1; k < n - 2; k++) {
                symbol += " " + parts.at(k);
            }
            cout << "  " << symbol << "  |  " << parts.at(n - 2) << "  | " << parts.at(n - 1) << endl;
        }
        string bits, back;
        h.encode(line, bits);
        h.decode(bits, back);
        cout << "Код: " << bits << endl;
        cout << "Длина: " << bits.size() << " бит (без сжатия " << line.size() * 8 << " бит)" << endl;
        cout << "Декодирование: " << back << endl;
    }
}
