#include "singly_list.hpp"

using namespace std;

SinglyList::SinglyList() {
    head = nullptr;
    tail = nullptr;
    count = 0;
}

SinglyList::~SinglyList() {
    while (head != nullptr) {
        Node* old = head;
        head = head->next;
        delete old;
    }
}

void SinglyList::push_front(const string& value) {
    Node* node = new Node{value, head};
    head = node;
    if (tail == nullptr) {
        tail = node;
    }
    count++;
}

void SinglyList::push_back(const string& value) {
    Node* node = new Node{value, nullptr};
    if (tail == nullptr) {
        head = node;
    } else {
        tail->next = node;
    }
    tail = node;
    count++;
}

bool SinglyList::insert_before(const string& key, const string& value) {
    if (head == nullptr) {
        return false;
    }
    if (head->value == key) {
        push_front(value);
        return true;
    }
    Node* previous = head;
    while (previous->next != nullptr && previous->next->value != key) {
        previous = previous->next;
    }
    if (previous->next == nullptr) {
        return false;
    }
    Node* node = new Node{value, previous->next};
    previous->next = node;
    count++;
    return true;
}

bool SinglyList::pop_front(string& removed) {
    if (head == nullptr) {
        return false;
    }
    Node* old = head;
    removed = old->value;
    head = head->next;
    if (head == nullptr) {
        tail = nullptr;
    }
    delete old;
    count--;
    return true;
}

bool SinglyList::remove(const string& key) {
    if (head == nullptr) {
        return false;
    }
    if (head->value == key) {
        string removed;
        return pop_front(removed);
    }
    Node* previous = head;
    while (previous->next != nullptr && previous->next->value != key) {
        previous = previous->next;
    }
    if (previous->next == nullptr) {
        return false;
    }
    Node* old = previous->next;
    previous->next = old->next;
    if (old == tail) {
        tail = previous;
    }
    delete old;
    count--;
    return true;
}

bool SinglyList::contains(const string& key) const {
    for (Node* current = head; current != nullptr; current = current->next) {
        if (current->value == key) {
            return true;
        }
    }
    return false;
}

Array SinglyList::values() const {
    Array result;
    for (Node* current = head; current != nullptr; current = current->next) {
        result.push_back(current->value);
    }
    return result;
}

SinglyList::SinglyList(SinglyList&& other) noexcept {
    head = other.head;
    tail = other.tail;
    count = other.count;
    other.head = nullptr;
    other.tail = nullptr;
    other.count = 0;
}

SinglyList& SinglyList::operator=(SinglyList&& other) noexcept {
    if (this != &other) {
        string removed;
        while (pop_front(removed)) {
        }
        head = other.head;
        tail = other.tail;
        count = other.count;
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }
    return *this;
}

int SinglyList::length() const {
    return count;
}

bool SinglyList::empty() const {
    return count == 0;
}
