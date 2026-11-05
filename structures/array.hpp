#pragma once
#include <string>
#include <utility>

using namespace std;

template <typename T>
class DynArray {
    T* data;
    int size;
    int capacity;

    void resize() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < size; i++) {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    DynArray() {
        capacity = 4;
        size = 0;
        data = new T[capacity];
    }

    DynArray(int n, const T& value) : DynArray() {
        for (int i = 0; i < n; i++) {
            push_back(value);
        }
    }

    ~DynArray() {
        delete[] data;
    }

    DynArray(const DynArray& other) {
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    DynArray& operator=(const DynArray& other) {
        if (this == &other) {
            return *this;
        }
        delete[] data;
        capacity = other.capacity;
        size = other.size;
        data = new T[capacity];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    DynArray(DynArray&& other) noexcept : data(other.data), size(other.size), capacity(other.capacity) {
        other.data = new T[4];
        other.size = 0;
        other.capacity = 4;
    }

    DynArray& operator=(DynArray&& other) noexcept {
        if (this != &other) {
            std::swap(data, other.data);
            std::swap(size, other.size);
            std::swap(capacity, other.capacity);
        }
        return *this;
    }

    void push_back(const T& value) {
        if (size == capacity) {
            resize();
        }
        data[size] = value;
        size++;
    }

    void push_back(T&& value) {
        if (size == capacity) {
            resize();
        }
        data[size] = std::move(value);
        size++;
    }

    bool set(int index, const T& value) {
        if (index < 0 || index >= size) {
            return false;
        }
        data[index] = value;
        return true;
    }

    int length() const {
        return size;
    }

    T at(int index) const {
        return data[index];
    }

    T& ref(int index) {
        return data[index];
    }

    const T& ref(int index) const {
        return data[index];
    }
};

using Array = DynArray<string>;

template <typename T, typename Less>
DynArray<T> MergeSort(const DynArray<T>& a, Less less) {
    int n = a.length();
    if (n <= 1) {
        return a;
    }
    DynArray<T> left, right;
    for (int i = 0; i < n; i++) {
        if (i < n / 2) {
            left.push_back(a.ref(i));
        } else {
            right.push_back(a.ref(i));
        }
    }
    left = MergeSort(left, less);
    right = MergeSort(right, less);
    DynArray<T> result;
    int i = 0, j = 0;
    while (i < left.length() && j < right.length()) {
        if (less(right.ref(j), left.ref(i))) {
            result.push_back(right.ref(j++));
        } else {
            result.push_back(left.ref(i++));
        }
    }
    while (i < left.length()) {
        result.push_back(left.ref(i++));
    }
    while (j < right.length()) {
        result.push_back(right.ref(j++));
    }
    return result;
}

template <typename T>
DynArray<T> MergeSort(const DynArray<T>& a) {
    return MergeSort(a, [](const T& x, const T& y) { return x < y; });
}
