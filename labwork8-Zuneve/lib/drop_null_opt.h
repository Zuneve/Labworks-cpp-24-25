#pragma once

inline auto DropNullopt() {
    return [](auto &&range) {
        return range
        | Filter([](auto &&v) {
            return v.has_value();
        })
        | Transform([](auto &&v) {
            return v.value();
        });
    };
}
