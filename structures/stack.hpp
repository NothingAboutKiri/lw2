#pragma once
#include "singly_list.hpp"

using namespace std;

class Stack {
    SinglyList list;
public:
    void push(const string& value);
    bool pop(string& removed);
    bool empty() const;
};
