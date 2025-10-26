#pragma once

#include <expected>
#include <filter.h>
#include <transform.h>
#include <processing.h>

inline auto SplitExpected() {
    return [](auto &&range) {
        auto unexpected_flow = range | Filter([](const auto &exp) {;
            return !exp.has_value();
        }) | Transform([](const auto &exp) {
            return exp.error();
        });

        auto good_flow = range | Filter([](const auto &exp) {
            return exp.has_value();
        }) | Transform([](const auto &exp) {
            return exp.value();
        });

        return std::make_pair(unexpected_flow, good_flow);
    };
}
