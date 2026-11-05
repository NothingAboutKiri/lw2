#include "pattern.hpp"
#include "../structures/array.hpp"

using namespace std;

bool MatchPattern(const string& text, const string& pattern) {
    DynArray<char> s, p;
    for (char c : text) s.push_back(c);
    for (char c : pattern) p.push_back(c);
    int n = s.length(), m = p.length();

    DynArray<bool> prev(m + 1, false);
    prev.set(0, true);
    for (int j = 1; j <= m; j++) {
        prev.set(j, prev.at(j - 1) && p.at(j - 1) == '*');
    }
    for (int i = 1; i <= n; i++) {
        DynArray<bool> cur(m + 1, false);
        for (int j = 1; j <= m; j++) {
            char pc = p.at(j - 1);
            bool v;
            if (pc == '*') {
                v = prev.at(j) || cur.at(j - 1);
            } else if (pc == '?' || pc == s.at(i - 1)) {
                v = prev.at(j - 1);
            } else {
                v = false;
            }
            cur.set(j, v);
        }
        prev = cur;
    }
    return prev.at(m);
}
