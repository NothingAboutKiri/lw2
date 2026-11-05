#include <fstream>
#include <sstream>
#include "json.hpp"

using namespace std;

static string JsonString(const string& text) {
    const char* hex = "0123456789abcdef";
    string result = "\"";
    for (int i = 0; i < (int)text.size(); i++) {
        unsigned char c = text[i];
        if (c == '"' || c == '\\') {
            result += '\\';
            result += (char)c;
        } else if (c < 0x20) {
            result += "\\u00";
            result += hex[c / 16];
            result += hex[c % 16];
        } else {
            result += (char)c;
        }
    }
    return result + "\"";
}

static void SkipSpaces(const string& text, int& pos) {
    while (pos < (int)text.size() &&
           (text[pos] == ' ' || text[pos] == '\t' || text[pos] == '\n' || text[pos] == '\r')) {
        pos++;
    }
}

static bool Expect(const string& text, int& pos, char c) {
    SkipSpaces(text, pos);
    if (pos < (int)text.size() && text[pos] == c) {
        pos++;
        return true;
    }
    return false;
}

static void AddUtf8(string& out, int code) {
    if (code < 0x80) {
        out += (char)code;
    } else if (code < 0x800) {
        out += (char)(0xC0 | (code >> 6));
        out += (char)(0x80 | (code & 0x3F));
    } else {
        out += (char)(0xE0 | (code >> 12));
        out += (char)(0x80 | ((code >> 6) & 0x3F));
        out += (char)(0x80 | (code & 0x3F));
    }
}

static bool ReadString(const string& text, int& pos, string& out) {
    out = "";
    if (!Expect(text, pos, '"')) {
        return false;
    }
    while (pos < (int)text.size()) {
        char c = text[pos++];
        if (c == '"') {
            return true;
        }
        if (c != '\\') {
            out += c;
            continue;
        }
        if (pos >= (int)text.size()) {
            return false;
        }
        char e = text[pos++];
        switch (e) {
            case '"': out += '"'; break;
            case '\\': out += '\\'; break;
            case '/': out += '/'; break;
            case 'n': out += '\n'; break;
            case 't': out += '\t'; break;
            case 'r': out += '\r'; break;
            case 'b': out += '\b'; break;
            case 'f': out += '\f'; break;
            case 'u': {
                if (pos + 4 > (int)text.size()) {
                    return false;
                }
                int code = 0;
                for (int i = 0; i < 4; i++) {
                    char h = text[pos++];
                    code *= 16;
                    if (h >= '0' && h <= '9') code += h - '0';
                    else if (h >= 'a' && h <= 'f') code += h - 'a' + 10;
                    else if (h >= 'A' && h <= 'F') code += h - 'A' + 10;
                    else return false;
                }
                if (code >= 0xD800 && code <= 0xDFFF) {
                    return false;
                }
                AddUtf8(out, code);
                break;
            }
            default:
                return false;
        }
    }
    return false;
}

static bool ReadValue(const string& text, int& pos, string& out) {
    SkipSpaces(text, pos);
    if (pos < (int)text.size() && text[pos] == '"') {
        return ReadString(text, pos, out);
    }
    out = "";
    while (pos < (int)text.size() && text[pos] != ',' && text[pos] != ']' &&
           text[pos] != ' ' && text[pos] != '\n' && text[pos] != '\r' && text[pos] != '\t') {
        out += text[pos++];
    }
    return !out.empty();
}

static bool ParseJson(const string& text, map<string, Array>& result) {
    int pos = 0;
    if (!Expect(text, pos, '{')) {
        return false;
    }
    if (Expect(text, pos, '}')) {
        SkipSpaces(text, pos);
        return pos == (int)text.size();
    }
    while (true) {
        string name;
        if (!ReadString(text, pos, name) || !Expect(text, pos, ':') || !Expect(text, pos, '[')) {
            return false;
        }
        Array values;
        if (!Expect(text, pos, ']')) {
            while (true) {
                string value;
                if (!ReadValue(text, pos, value)) {
                    return false;
                }
                values.push_back(value);
                if (Expect(text, pos, ']')) {
                    break;
                }
                if (!Expect(text, pos, ',')) {
                    return false;
                }
            }
        }
        result[name] = values;
        if (Expect(text, pos, '}')) {
            break;
        }
        if (!Expect(text, pos, ',')) {
            return false;
        }
    }
    SkipSpaces(text, pos);
    return pos == (int)text.size();
}

bool LoadJson(const string& path, map<string, Array>& result, bool& exists) {
    ifstream file(path);
    exists = file.is_open();
    if (!exists) {
        return true;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    map<string, Array> parsed;
    if (!ParseJson(buffer.str(), parsed)) {
        return false;
    }
    result = parsed;
    return true;
}

bool SaveJson(const string& path, const map<string, Array>& items) {
    ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file << "{\n";
    bool first = true;
    for (const auto& item : items) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        file << "  " << JsonString(item.first) << ": [";
        for (int i = 0; i < item.second.length(); i++) {
            if (i > 0) {
                file << ", ";
            }
            file << JsonString(item.second.at(i));
        }
        file << "]";
    }
    if (!first) {
        file << "\n";
    }
    file << "}\n";
    return true;
}
