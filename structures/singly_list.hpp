#pragma once
#include <string>
#include "array.hpp"

using namespace std;

class SinglyList {
    struct Node {
        string value;
        Node* next;
    };
    Node* head;
    Node* tail;
    int count;

public:
    SinglyList();
    ~SinglyList();
    SinglyList(const SinglyList&) = delete;
    SinglyList& operator=(const SinglyList&) = delete;
    SinglyList(SinglyList&& other) noexcept;
    SinglyList& operator=(SinglyList&& other) noexcept;

    void push_front(const string& value);
    void push_back(const string& value);
    bool insert_before(const string& key, const string& value);
    bool pop_front(string& removed);
    bool remove(const string& key);
    bool contains(const string& key) const;
    Array values() const;
    int length() const;
    bool empty() const;
};
