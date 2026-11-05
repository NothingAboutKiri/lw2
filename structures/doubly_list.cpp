#include "doubly_list.hpp"

using namespace std;

DoublyList::DoublyList() {
    head = nullptr;
    tail = nullptr;
    count = 0;
}

DoublyList::~DoublyList() {
    while (head != nullptr) {
        Node* old = head;
        head = head->next;
        delete old;
    }
}

DoublyList::Node* DoublyList::push_front(const string& value) {
    Node* node = new Node{value, nullptr, head};
    if (head != nullptr) {
        head->prev = node;
    } else {
        tail = node;
    }
    head = node;
    count++;
    return node;
}

void DoublyList::remove(Node* node) {
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        head = node->next;
    }
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        tail = node->prev;
    }
    delete node;
    count--;
}

DoublyList::Node* DoublyList::back() const {
    return tail;
}

int DoublyList::length() const {
    return count;
}

Array DoublyList::values() const {
    Array result;
    for (Node* current = head; current != nullptr; current = current->next) {
        result.push_back(current->value);
    }
    return result;
}
