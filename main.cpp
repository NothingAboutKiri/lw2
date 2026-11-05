#include <iostream>
#include "tasks/io.hpp"
#include "tasks/tasks.hpp"

using namespace std;

void PrintMenu() {
    cout << "\n"
            "+-----+----------------------------------------------+\n"
            "| No  | Задание                                      |\n"
            "+-----+----------------------------------------------+\n"
            "| 1   | Стек: восстановление XML-строки              |\n"
            "| 2   | Множество: SETADD, SETDEL, SET_AT,           |\n"
            "|     | объединение, пересечение, разность (зад. 3)  |\n"
            "| 4   | Массив: сопоставление с шаблоном             |\n"
            "| 5   | Дерево: алгоритм Хаффмана                    |\n"
            "| 6.1 | Хеш-таблица: открытая адресация, анализ      |\n"
            "| 6.2 | Хеш-таблица: родословная                     |\n"
            "| 7   | LRU-кэш                                      |\n"
            "| 0   | Выход                                        |\n"
            "+-----+----------------------------------------------+" << endl;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        return RunSetCli(argc, argv);
    }
    string choice;
    while (true) {
        PrintMenu();
        if (!Ask("Выбор: ", choice) || choice == "0") return 0;
        if (choice == "1") RunXml();
        else if (choice == "2" || choice == "3") RunSets();
        else if (choice == "4") RunPattern();
        else if (choice == "5") RunHuffman();
        else if (choice == "6.1" || choice == "6") RunHash();
        else if (choice == "6.2") RunGenealogy();
        else if (choice == "7") RunLru();
        else cout << "Ошибка: такого пункта нет" << endl;
    }
}
