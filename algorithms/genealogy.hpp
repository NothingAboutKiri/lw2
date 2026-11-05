#pragma once
#include <string>
#include "../structures/array.hpp"
#include "../structures/open_hash.hpp"
#include "../structures/singly_list.hpp"

using namespace std;

class FamilyTree {
    OpenHash<int> index;
    Array names;
    DynArray<int> parent;
    DynArray<SinglyList> children;

    int id(const string& name);
    int number(const string& name) const;
    bool root(int& r, string& error) const;

public:
    FamilyTree();
    bool add(const string& child, const string& par, string& error);
    int size() const;
    bool descendants(DynArray<int>& count, string& error) const;
    bool report(Array& out, string& error) const;
    bool draw(Array& out, string& error) const;
};
