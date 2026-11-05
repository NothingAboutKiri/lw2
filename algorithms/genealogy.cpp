#include "genealogy.hpp"
#include "../structures/stack.hpp"

using namespace std;

FamilyTree::FamilyTree() : index(Method::Double, 16) {}

int FamilyTree::id(const string& name) {
    int i;
    if (index.get(name, i)) return i;
    i = names.length();
    index.insert(name, i);
    names.push_back(name);
    parent.push_back(-1);
    children.push_back(SinglyList());
    return i;
}

int FamilyTree::number(const string& name) const {
    int i = -1;
    index.get(name, i);
    return i;
}

bool FamilyTree::add(const string& child, const string& par, string& error) {
    if (child == par) {
        error = child + " не может быть своим родителем";
        return false;
    }
    int c = id(child);
    int p = id(par);
    if (parent.at(c) != -1) {
        error = "у " + child + " уже есть родитель";
        return false;
    }
    parent.set(c, p);
    children.ref(p).push_back(child);
    return true;
}

int FamilyTree::size() const {
    return names.length();
}

bool FamilyTree::root(int& r, string& error) const {
    r = -1;
    for (int i = 0; i < size(); i++) {
        if (parent.at(i) == -1) {
            if (r != -1) {
                error = "у древа больше одного родоначальника";
                return false;
            }
            r = i;
        }
    }
    if (r == -1) {
        error = "нет родоначальника (связи образуют цикл)";
        return false;
    }
    return true;
}

bool FamilyTree::descendants(DynArray<int>& count, string& error) const {
    int r;
    if (!root(r, error)) return false;
    DynArray<int> order;
    Stack stack;
    stack.push(to_string(r));
    string top;
    while (stack.pop(top)) {
        int v = stoi(top);
        order.push_back(v);
        Array kids = children.ref(v).values();
        for (int k = 0; k < kids.length(); k++) {
            stack.push(to_string(number(kids.at(k))));
        }
    }
    if (order.length() != size()) {
        error = "древо несвязно (есть цикл)";
        return false;
    }
    count = DynArray<int>(size(), 0);
    for (int k = order.length() - 1; k >= 0; k--) {
        int v = order.at(k);
        int s = 0;
        Array kids = children.ref(v).values();
        for (int j = 0; j < kids.length(); j++) {
            s += 1 + count.at(number(kids.at(j)));
        }
        count.set(v, s);
    }
    return true;
}

bool FamilyTree::report(Array& out, string& error) const {
    DynArray<int> count;
    if (!descendants(count, error)) return false;
    Array sorted = MergeSort(names);
    out = Array();
    for (int i = 0; i < sorted.length(); i++) {
        out.push_back(sorted.at(i) + " " + to_string(count.at(number(sorted.at(i)))));
    }
    return true;
}

bool FamilyTree::draw(Array& out, string& error) const {
    int r;
    if (!root(r, error)) return false;
    out = Array();
    Stack stack;
    stack.push("0 " + names.at(r));
    string top;
    while (stack.pop(top)) {
        size_t space = top.find(' ');
        int depth = stoi(top.substr(0, space));
        string name = top.substr(space + 1);
        out.push_back(string(2 * depth, ' ') + name);
        Array kids = MergeSort(children.ref(number(name)).values());
        for (int k = kids.length() - 1; k >= 0; k--) {
            stack.push(to_string(depth + 1) + " " + kids.at(k));
        }
    }
    return true;
}
