//
// Created by alex on 14.10.20.
//

#include "bcg_string_utils.h"

namespace bcg {

// see https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
void ltrim(std::string &s, char c) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [c](int ch) {
        return !(ch == c);
    }));
}

void rtrim(std::string &s, char c) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [c](int ch) {
        return !(ch == c);
    }).base(), s.end());
}

void trim(std::string &s, char c) {
    ltrim(s, c);
    rtrim(s, c);
}

std::string join(const std::string &str1, const std::string &str2) {
    return std::string(str1 + str2);
}

std::vector<std::string> split(const std::string &str, const std::vector<char> &delims) {
    size_t start = 0;
    std::vector<std::string> tokens;
    for (size_t i = 0; i < str.size(); ++i) {
        if (std::find(delims.begin(), delims.end(), str[i]) != delims.end()) {
            if (start < i) {
                tokens.push_back(str.substr(start, i - start));
            }
            start = i + 1;
        }
    }

    if (start < str.size()) {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}

std::vector<std::string> split(const std::string &str, char delim) {
    std::vector<char> delims = {delim};
    return split(str, delims);
}

void remove_chars(std::string &s, const std::string &chars) {
    s.erase(std::remove_if(s.begin(), s.end(), [&chars](const char &c) {
        return chars.find(c) != std::string::npos;
    }), s.end());
}

void remove_non_digits(std::string &s) {
    s.erase(std::remove_if(s.begin(), s.end(), [](const char &c) {
        return !std::isdigit(c);
    }), s.end());
}

}