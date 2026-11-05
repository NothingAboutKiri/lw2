#pragma once
#include <map>
#include <string>
#include "../structures/array.hpp"
#include "../structures/hash_set.hpp"

using namespace std;

string SolveXml(const string& line);
void RunXml();

struct SetStore {
    map<string, HashSet> sets;
    bool load(const string& path, string& error);
    bool save(const string& path) const;
    string execute(const string& query, bool& changed);
};
int RunSetCli(int argc, char* argv[]);
void RunSets();

string SolvePattern(const string& text, const string& pattern);
void RunPattern();

void RunHuffman();

string ExperimentTable(int m);
void RunHash();

bool SolveGenealogy(int n, const Array& pairs, string& out, string& error);
void RunGenealogy();

bool SolveLru(int cap, const Array& tokens, string& out, string& state, string& error);
void RunLru();
