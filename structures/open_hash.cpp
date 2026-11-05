#include <cmath>
#include "open_hash.hpp"

using namespace std;

bool IsPrime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; d++) {
        if (n % d == 0) return false;
    }
    return true;
}

int NextPrime(int n) {
    while (!IsPrime(n)) n++;
    return n;
}

Rng::Rng(unsigned long long seed) {
    x = (seed * 6364136223846793005ULL + 1442695040888963407ULL) | 1;
}

unsigned long long Rng::next() {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return x;
}

Measure Experiment(Method method, int m, double alpha, int runs) {
    double succTotal = 0, unsTotal = 0;
    for (int run = 0; run < runs; run++) {
        Rng rng(run + 1);
        OpenHash<string> t(method, m);
        t.autoResize = false;
        int n = (int)(alpha * t.capacity());
        Array keys;
        while (t.size() < n) {
            string k = "k" + to_string(rng.next() % 1000000000ULL);
            if (!t.contains(k)) {
                t.insert(k, "1");
                keys.push_back(k);
            }
        }
        long long s = 0;
        for (int i = 0; i < keys.length(); i++) {
            s += t.search_probes(keys.at(i));
        }
        succTotal += (double)s / n;
        const int queries = 2000;
        long long u = 0;
        for (int q = 0; q < queries; q++) {
            u += t.search_probes("u" + to_string(rng.next() % 1000000000ULL));
        }
        unsTotal += (double)u / queries;
    }
    return {succTotal / runs, unsTotal / runs};
}

Measure Theory(Method method, double a) {
    if (method == Method::Linear) {
        return {0.5 * (1 + 1 / (1 - a)), 0.5 * (1 + 1 / ((1 - a) * (1 - a)))};
    }
    return {(1 / a) * log(1 / (1 - a)), 1 / (1 - a)};
}
