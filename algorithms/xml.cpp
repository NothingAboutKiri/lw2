#include "xml.hpp"
#include "../structures/stack.hpp"

using namespace std;

static bool IsName(const string& s, int from, int to) {
    if (from >= to) return false;
    for (int i = from; i < to; i++) {
        if (s[i] < 'a' || s[i] > 'z') return false;
    }
    return true;
}

bool IsValidXml(const string& s) {
    int n = (int)s.size();
    Stack stack;
    int i = 0;
    while (i < n) {
        if (s[i] != '<') return false;
        int j = i + 1;
        while (j < n && s[j] != '>') j++;
        if (j == n) return false;
        if (j > i + 1 && s[i + 1] == '/') {
            if (!IsName(s, i + 2, j)) return false;
            string top;
            if (!stack.pop(top) || top != s.substr(i + 2, j - i - 2)) return false;
        } else {
            if (!IsName(s, i + 1, j)) return false;
            stack.push(s.substr(i + 1, j - i - 1));
        }
        i = j + 1;
    }
    return stack.empty();
}

bool RestoreXml(const string& s, string& out) {
    const string alphabet = "abcdefghijklmnopqrstuvwxyz<>/";
    string t = s;
    for (int i = (int)t.size() - 1; i >= 0; i--) {
        char original = t[i];
        for (char c : alphabet) {
            if (c == original) continue;
            t[i] = c;
            if (IsValidXml(t)) {
                out = t;
                return true;
            }
        }
        t[i] = original;
    }
    return false;
}
