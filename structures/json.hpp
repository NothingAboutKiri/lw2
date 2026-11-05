#pragma once
#include <map>
#include <string>
#include "array.hpp"

using namespace std;

bool LoadJson(const string& path, map<string, Array>& result, bool& exists);
bool SaveJson(const string& path, const map<string, Array>& items);
