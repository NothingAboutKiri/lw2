#include <cstdlib>
#include "hash_set.hpp"

using namespace std;

static const int START_BUCKETS = 8;

unsigned long long HashString(const string& text) {
    unsigned long long h = 0;
    for (unsigned char c : text) {
        h = h * 31 + c;   // переполнение = взятие по модулю 2^64
    }
    return h;
}

HashSet::HashSet() {
    for (int i = 0; i < START_BUCKETS; i++) {
        buckets.push_back(SinglyList());
    }
    count = 0;
}

int HashSet::index(const string& x) const {
    return (int)(HashString(x) % (unsigned long long)buckets.length());
}

// Перехеширование: переносим элементы, а не содержимое старых ячеек по их номерам.
void HashSet::rehash() {
    Array elements = values();
    int newSize = buckets.length() * 2;
    buckets = DynArray<SinglyList>();
    for (int i = 0; i < newSize; i++) {
        buckets.push_back(SinglyList());
    }
    // Счётчик восстановится при повторной вставке элементов.
    count = 0;
    for (int i = 0; i < elements.length(); i++) {
        // add() вычисляет хеш-индекс каждого элемента по новому размеру.
        add(elements.at(i));
    }
}

bool HashSet::add(const string& x) {
    if (contains(x)) {
        return false;
    }
    if ((count + 1) * 4 > buckets.length() * 3) {   // заполнение больше 0,75
        rehash();
    }
    buckets.ref(index(x)).push_front(x);
    count++;
    return true;
}

bool HashSet::remove(const string& x) {
    if (buckets.ref(index(x)).remove(x)) {
        count--;
        return true;
    }
    return false;
}

bool HashSet::contains(const string& x) const {
    return buckets.ref(index(x)).contains(x);
}

int HashSet::size() const {
    return count;
}

Array HashSet::values() const {
    Array result;
    for (int i = 0; i < buckets.length(); i++) {
        Array chain = buckets.ref(i).values();
        for (int j = 0; j < chain.length(); j++) {
            result.push_back(chain.at(j));
        }
    }
    return result;
}

// Целое число без знака или со знаком минус
static bool IsNumber(const string& s, long long& value) {
    if (s.empty() || s.size() > 18) {
        return false;
    }
    int i = (s[0] == '-') ? 1 : 0;
    if (i == (int)s.size()) {
        return false;
    }
    for (int k = i; k < (int)s.size(); k++) {
        if (s[k] < '0' || s[k] > '9') {
            return false;
        }
    }
    value = atoll(s.c_str());
    return true;
}

// Числа - численно и раньше слов, слова - по алфавиту
static bool LessValue(const string& a, const string& b) {
    long long x, y;
    bool na = IsNumber(a, x), nb = IsNumber(b, y);
    if (na && nb) return x < y;
    if (na != nb) return na;
    return a < b;
}

Array HashSet::sorted() const {
    return MergeSort(values(), LessValue);
}

string HashSet::show() const {
    Array s = sorted();
    string out = "{";
    for (int i = 0; i < s.length(); i++) {
        if (i > 0) out += ", ";
        out += s.at(i);
    }
    return out + "}";
}

// Объединение A ∪ B, O(|A| + |B|)
HashSet HashSet::unite(const HashSet& other) const {
    HashSet c;
    Array a = values(), b = other.values();
    for (int i = 0; i < a.length(); i++) c.add(a.at(i));
    for (int i = 0; i < b.length(); i++) c.add(b.at(i));
    return c;
}

// Пересечение A ∩ B: перебирается меньшее множество, O(min(|A|, |B|))
HashSet HashSet::intersect(const HashSet& other) const {
    const HashSet& small = (size() <= other.size()) ? *this : other;
    const HashSet& big = (size() <= other.size()) ? other : *this;
    HashSet c;
    Array s = small.values();
    for (int i = 0; i < s.length(); i++) {
        if (big.contains(s.at(i))) c.add(s.at(i));
    }
    return c;
}

// Разность A \ B, O(|A|)
HashSet HashSet::subtract(const HashSet& other) const {
    HashSet c;
    Array a = values();
    for (int i = 0; i < a.length(); i++) {
        if (!other.contains(a.at(i))) c.add(a.at(i));
    }
    return c;
}
