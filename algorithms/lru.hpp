#pragma once
#include <string>
#include "../structures/array.hpp"
#include "../structures/doubly_list.hpp"
#include "../structures/open_hash.hpp"

using namespace std;

class LruCache {
    int cap;
    DoublyList order;
    OpenHash<DoublyList::Node*> node;
    OpenHash<string> value;

    void touch(const string& key);

public:
    explicit LruCache(int cap);
    bool get(const string& key, string& val);
    void set(const string& key, const string& val);
    Array state() const;
};
