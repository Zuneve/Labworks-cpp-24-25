#pragma once

#include <ostream>
#include <string>

inline auto Out(std::ostream& out, const std::string& separator = "\n") {
    return [&out, &separator](auto&& range) {
        for (const auto& elem : range) {
            out << elem << separator;
        }
    };
}
