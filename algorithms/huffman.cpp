#include "huffman.hpp"
#include "../structures/singly_list.hpp"
#include "../structures/stack.hpp"

using namespace std;

Huffman::Huffman() : freq(Method::Double, 16), codes(Method::Double, 16) {}

static void InsertSorted(SinglyList& free, const DynArray<HuffmanNode>& nodes, int idx) {
    Array v = free.values();
    for (int i = 0; i < v.length(); i++) {
        if (nodes.ref(stoi(v.at(i))).weight > nodes.ref(idx).weight) {
            free.insert_before(v.at(i), to_string(idx));
            return;
        }
    }
    free.push_back(to_string(idx));
}

bool Huffman::build(const string& text) {
    if (text.empty()) return false;
    nodes = DynArray<HuffmanNode>();
    symbols = Array();
    freq = OpenHash<int>(Method::Double, 16);
    codes = OpenHash<string>(Method::Double, 16);

    for (char c : text) {
        string key(1, c);
        int w;
        if (freq.get(key, w)) {
            freq.insert(key, w + 1);
        } else {
            freq.insert(key, 1);
            symbols.push_back(key);
        }
    }
    SinglyList free;
    for (int i = 0; i < symbols.length(); i++) {
        HuffmanNode leaf;
        leaf.symbol = symbols.at(i);
        freq.get(leaf.symbol, leaf.weight);
        nodes.push_back(leaf);
        InsertSorted(free, nodes, nodes.length() - 1);
    }
    string a, b;
    while (free.length() > 1) {
        free.pop_front(a);
        free.pop_front(b);
        int ia = stoi(a), ib = stoi(b);
        HuffmanNode parent;
        parent.symbol = nodes.ref(ia).symbol + nodes.ref(ib).symbol;
        parent.weight = nodes.ref(ia).weight + nodes.ref(ib).weight;
        parent.left = ia;
        parent.right = ib;
        nodes.push_back(parent);
        InsertSorted(free, nodes, nodes.length() - 1);
    }
    free.pop_front(a);
    root = stoi(a);

    Stack stack;
    stack.push(to_string(root) + " ");
    string item;
    while (stack.pop(item)) {
        size_t space = item.find(' ');
        int idx = stoi(item.substr(0, space));
        string code = item.substr(space + 1);
        const HuffmanNode& node = nodes.ref(idx);
        if (node.left >= 0) {
            stack.push(to_string(node.right) + " " + code + "1");
            stack.push(to_string(node.left) + " " + code + "0");
        } else {
            codes.insert(node.symbol, code.empty() ? "0" : code);
        }
    }
    return true;
}

string Huffman::code_of(const string& symbol) const {
    string code;
    codes.get(symbol, code);
    return code;
}

int Huffman::weight_of(const string& symbol) const {
    int w = 0;
    freq.get(symbol, w);
    return w;
}

bool Huffman::encode(const string& text, string& bits) const {
    bits = "";
    for (char c : text) {
        string code;
        if (!codes.get(string(1, c), code)) return false;
        bits += code;
    }
    return true;
}

bool Huffman::decode(const string& bits, string& text) const {
    text = "";
    if (root < 0) return false;
    const HuffmanNode& r = nodes.ref(root);
    if (r.left < 0) {
        for (char b : bits) {
            if (b != '0') return false;
            text += r.symbol;
        }
        return true;
    }
    int cur = root;
    for (char b : bits) {
        if (b == '0') cur = nodes.ref(cur).left;
        else if (b == '1') cur = nodes.ref(cur).right;
        else return false;
        if (nodes.ref(cur).left < 0) {
            text += nodes.ref(cur).symbol;
            cur = root;
        }
    }
    return cur == root;
}

Array Huffman::table() const {
    Array out;
    for (int i = 0; i < symbols.length(); i++) {
        string s = symbols.at(i);
        out.push_back((s == " " ? "' '" : s) + " " + to_string(weight_of(s)) + " " + code_of(s));
    }
    return out;
}
