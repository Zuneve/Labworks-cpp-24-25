#pragma once

#include <functional>
#include <memory>
#include <algorithm>
struct pair_hash {
    std::size_t operator ()(const std::pair<uint64_t, uint64_t> &pair) const {
        auto hash1 = std::hash<uint64_t>{}(pair.first);
        auto hash2 = std::hash<uint64_t>{}(pair.second);
        return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
    }
};
