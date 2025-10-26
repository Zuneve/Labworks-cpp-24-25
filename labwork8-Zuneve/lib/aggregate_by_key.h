#pragma once

#include <map>
#include <vector>
#include <utility>
#include <type_traits>

template <typename AggregateType, typename AggregateFunction, typename KeyFunction>
class AggregateByKeyAdapter {
public:
    AggregateByKeyAdapter(const AggregateType& init, AggregateFunction aggregator, KeyFunction key_func)
        : init_(std::move(init)), aggregator_(std::move(aggregator)), key_func_(std::move(key_func)) {}

    template <typename Range>
    auto operator()(Range&& range) {
        using Source = typename std::decay_t<Range>::value_type;
        using KeyType = decltype(key_func_(std::declval<Source>()));

        std::map<KeyType, AggregateType> result;

        for (auto&& elem : range) {
            KeyType key = key_func_(elem);
            aggregator_(elem, result[key]);
        }

        std::vector<std::pair<KeyType, AggregateType>> vec(result.begin(), result.end());
        return vec;
    }

private:
    AggregateType init_;
    AggregateFunction aggregator_;
    KeyFunction key_func_;
};

template <typename AggregateType, typename AggregateFunction, typename KeyFunction>
inline auto AggregateByKey(const AggregateType& init, AggregateFunction aggregator, KeyFunction key_func) {
    return AggregateByKeyAdapter<AggregateType, AggregateFunction, KeyFunction>(init, aggregator, key_func);
}
