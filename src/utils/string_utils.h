#pragma once

#include <string>
#include <algorithm>
#include <cctype>

namespace kvstore::utils {
inline std::string Trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), 
                                   [](unsigned char ch) { return std::isspace(ch); });
    auto end = std::find_if_not(str.rbegin(), str.rend(),
                                 [](unsigned char ch) { return std::isspace(ch); }).base();
    
    return (start < end) ? std::string(start, end) : std::string();
}

inline std::string TrimLeft(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(),
                                   [](unsigned char ch) { return std::isspace(ch); });
    return std::string(start, str.end());
}

inline std::string TrimRight(const std::string& str) {
    auto end = std::find_if_not(str.rbegin(), str.rend(),
                                 [](unsigned char ch) { return std::isspace(ch); }).base();
    return std::string(str.begin(), end);
}

}