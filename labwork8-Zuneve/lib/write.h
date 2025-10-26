#pragma once

inline auto Write(std::ostream& out, char separator = '\n') {
    return [&out, separator](auto&& range) {
        for (const auto& elem : range) {
            out << elem << separator;
        }
        return range;
    };
}
