#pragma once
#include <string>
#include "../structures/array.hpp"
#include "../structures/open_hash.hpp"

using namespace std;

struct HuffmanNode {
    string symbol;
    int weight = 0;
    int left = -1;
    int right = -1;
};

class Huffman {
    DynArray<HuffmanNode> nodes;
    int root = -1;
    Array symbols;
    OpenHash<int> freq;
    OpenHash<string> codes;

public:
    Huffman();
    bool build(const string& text);
    bool encode(const string& text, string& bits) const;
    bool decode(const string& bits, string& text) const;
    Array table() const;
    string code_of(const string& symbol) const;
    int weight_of(const string& symbol) const;
};
