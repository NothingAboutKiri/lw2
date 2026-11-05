#pragma once
#include <string>
#include "array.hpp"
#include "singly_list.hpp"

using namespace std;

// Полиномиальный хеш строки: h = h * 31 + код символа (mod 2^64)
unsigned long long HashString(const string& text);

// Задания 2 и 3. Множество строк на хеш-таблице с методом цепочек.
// Таблица - массив DynArray, в каждой ячейке - односвязный список (цепочка) из ЛР №1.
class HashSet {
    DynArray<SinglyList> buckets;
    int count;

    int index(const string& x) const;   // номер ячейки
    void rehash();                      // увеличить таблицу вдвое

public:
    HashSet();
    bool add(const string& x);              // SETADD: false, если элемент уже есть
    bool remove(const string& x);           // SETDEL: false, если элемента нет
    bool contains(const string& x) const;   // SET_AT
    int size() const;
    Array values() const;                   // все элементы (обход всех цепочек)
    Array sorted() const;                   // по возрастанию (числа - как числа)
    string show() const;                    // {1, 2, 3}

    // Задание 3
    HashSet unite(const HashSet& other) const;       // объединение
    HashSet intersect(const HashSet& other) const;   // пересечение
    HashSet subtract(const HashSet& other) const;    // разность
};
