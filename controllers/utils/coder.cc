#include "coder.h"

std::string encode(const std::string &uncoded_str) {
    std::string str{uncoded_str};
    auto strLen{str.length()};
    for (int i = 0; i < strLen - 1; ++i) {
        str[i] ^= str[i + 1];
    }
    return str;
}

std::string decode(const std::string &str) {
    std::string encoded{str};
    for (int i = encoded.size() - 2; i >= 0; --i) {
        encoded[i] ^= encoded[i + 1];
    }
    return encoded;
}