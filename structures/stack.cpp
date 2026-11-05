#include "stack.hpp"

using namespace std;

void Stack::push(const string& value) {
    list.push_front(value);
}

bool Stack::pop(string& removed) {
    return list.pop_front(removed);
}

bool Stack::empty() const {
    return list.empty();
}
