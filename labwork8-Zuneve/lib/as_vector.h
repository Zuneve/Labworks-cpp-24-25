#pragma once

#include <vector>

inline auto AsVector() {
    return [](auto&& range) {
        using RangeType = std::decay_t<decltype(range)>;
        using ValueType = std::decay_t<typename RangeType::value_type>;
        std::vector<ValueType> vec;
        for (auto&& x : range) {
            vec.push_back(x);
        }
        return vec;
    };
}
