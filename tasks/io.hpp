#pragma once
#include <string>
#include "../structures/array.hpp"

using namespace std;

bool ReadLine(string& line);
bool Ask(const string& prompt, string& line);
Array SplitWords(const string& line);
string Upper(string s);
string Join(const Array& items, const string& sep);
