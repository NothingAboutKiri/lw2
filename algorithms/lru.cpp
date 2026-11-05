#include "lru.hpp"

using namespace std;

LruCache::LruCache(int cap) : cap(cap), node(Method::Double, cap * 2 + 3), value(Method::Double, cap * 2 + 3) {}

void LruCache::touch(const string& key) {
    DoublyList::Node* n = nullptr;
    node.get(key, n);
    order.remove(n);
    node.insert(key, order.push_front(key));
}

bool LruCache::get(const string& key, string& val) {
    if (!value.get(key, val)) return false;
    touch(key);
    return true;
}

void LruCache::set(const string& key, const string& val) {
    if (cap <= 0) return;
    if (value.contains(key)) {
        value.insert(key, val);
        touch(key);
        return;
    }
    if (order.length() == cap) {
        DoublyList::Node* tail = order.back();
        string old = tail->value;
        order.remove(tail);
        node.remove(old);
        value.remove(old);
    }
    node.insert(key, order.push_front(key));
    value.insert(key, val);
}

Array LruCache::state() const {
    Array keys = order.values();
    Array out;
    for (int i = keys.length() - 1; i >= 0; i--) {
        string v;
        value.get(keys.at(i), v);
        out.push_back(keys.at(i) + "->" + v);
    }
    return out;
}
