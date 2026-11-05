#pragma once
#include <stdexcept>
#include <string>
#include "array.hpp"
#include "hash_set.hpp"

using namespace std;

// Задание 6.1. Хеш-таблица «строка -> значение» с открытой адресацией.
// Методы разрешения коллизий: линейное апробирование и двойное хеширование.
// Ключи, значения и состояния ячеек хранятся в трёх массивах DynArray.
// Шаблон по типу значения: OpenHash<string>, OpenHash<int>, OpenHash<указатель на узел>.

enum class Method { Linear, Double };

bool IsPrime(int n);
int NextPrime(int n);

template <typename V>
class OpenHash {
    enum State : char { EMPTY = 0, BUSY = 1, DELETED = 2 };   // DELETED - «надгробие»

    Method method;
    DynArray<string> keys;
    DynArray<V> vals;
    DynArray<char> state;
    int count;     // занятые ячейки
    int deleted;   // надгробия

    // i-я проба для ключа: (h1 + i) mod m или (h1 + i * h2) mod m, h2 = 1 + H mod (m - 1)
    int probe(unsigned long long h, int i) const {
        unsigned long long m = state.length();
        unsigned long long h1 = h % m;
        unsigned long long step = (method == Method::Linear) ? 1 : 1 + h % (m - 1);
        return (int)((h1 + (unsigned long long)i * step) % m);
    }

    // Поиск ячейки ключа: номер или -1; probes - число проб
    int find(const string& key, int& probes) const {
        unsigned long long h = HashString(key);
        int m = capacity();
        for (int i = 0; i < m; i++) {
            int pos = probe(h, i);
            probes = i + 1;
            if (state.ref(pos) == EMPTY) return -1;
            if (state.ref(pos) == BUSY && keys.ref(pos) == key) return pos;
        }
        return -1;
    }

    // Перестроить таблицу размером newSize: все ключи вставляются заново
    void rebuild(int newSize) {
        OpenHash t(method, newSize);
        t.autoResize = false;
        for (int i = 0; i < capacity(); i++) {
            if (state.ref(i) == BUSY) t.insert(keys.ref(i), vals.ref(i));
        }
        t.autoResize = autoResize;
        *this = std::move(t);
    }

public:
    bool autoResize = true;   // увеличивать таблицу при заполнении больше 75 %

    // size округляется вверх до простого числа
    OpenHash(Method method, int size) : method(method), count(0), deleted(0) {
        int m = NextPrime(size < 3 ? 3 : size);
        keys = DynArray<string>(m, "");
        vals = DynArray<V>(m, V());
        state = DynArray<char>(m, EMPTY);
    }

    int capacity() const { return state.length(); }
    int size() const { return count; }
    double load_factor() const { return (double)count / capacity(); }

    // Вставка или замена значения; возвращает число проб
    int insert(const string& key, const V& value) {
        if (autoResize && (count + deleted + 1) * 4 > capacity() * 3) {
            rebuild((count + 1) * 2 > capacity() ? capacity() * 2 : capacity());
        }
        int probes = 0;
        int pos = find(key, probes);
        if (pos >= 0) {
            vals.set(pos, value);
            return probes;
        }
        unsigned long long h = HashString(key);
        for (int i = 0; i < capacity(); i++) {
            pos = probe(h, i);
            if (state.ref(pos) != BUSY) {
                if (state.ref(pos) == DELETED) deleted--;
                keys.set(pos, key);
                vals.set(pos, value);
                state.set(pos, BUSY);
                count++;
                return i + 1;
            }
        }
        throw runtime_error("хеш-таблица переполнена");
    }

    bool get(const string& key, V& value) const {
        int probes;
        int pos = find(key, probes);
        if (pos < 0) return false;
        value = vals.ref(pos);
        return true;
    }

    bool contains(const string& key) const {
        int probes;
        return find(key, probes) >= 0;
    }

    // Число проб при поиске ключа (для эксперимента)
    int search_probes(const string& key) const {
        int probes = 0;
        find(key, probes);
        return probes;
    }

    bool remove(const string& key) {
        int probes;
        int pos = find(key, probes);
        if (pos < 0) return false;
        state.set(pos, DELETED);
        keys.set(pos, "");
        vals.set(pos, V());
        count--;
        deleted++;
        return true;
    }

    // Строки "номер: ключ" для занятых ячеек и "номер: <deleted>" для надгробий
    Array dump() const {
        Array out;
        for (int i = 0; i < capacity(); i++) {
            if (state.ref(i) == BUSY) out.push_back(to_string(i) + ": " + keys.ref(i));
            if (state.ref(i) == DELETED) out.push_back(to_string(i) + ": <deleted>");
        }
        return out;
    }
};

// ---------- Эмпирический анализ ----------

// Генератор псевдослучайных чисел xorshift (опыт повторяем)
class Rng {
    unsigned long long x;
public:
    explicit Rng(unsigned long long seed);
    unsigned long long next();
};

struct Measure {
    double success;     // среднее число проб при успешном поиске
    double unsuccess;   // при неуспешном
};

// Заполнить таблицу размером m до коэффициента alpha, измерить среднее число проб (runs повторов)
Measure Experiment(Method method, int m, double alpha, int runs);
// Теоретические оценки Д. Кнута
Measure Theory(Method method, double alpha);
