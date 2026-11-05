#pragma once
#include <string>
#include "array.hpp"

using namespace std;

class DoublyList {
public:
    struct Node {
        string value;
        Node* prev;
        Node* next;
    };

private:
    Node* head;
    Node* tail;
    int count;

public:
    DoublyList();
    ~DoublyList();
    DoublyList(const DoublyList&) = delete;
    DoublyList& operator=(const DoublyList&) = delete;

    Node* push_front(const string& value);
    void remove(Node* node);
    Node* back() const;
    int length() const;
    Array values() const;
};
