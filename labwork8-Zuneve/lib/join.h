#pragma once

#include <map>
#include <optional>

#include "join_result.h"

template<typename T, typename = void>
struct GetJoinValue {
    using type = T;
    static const T& Get(const T &obj) { return obj; }
};

template<typename T>
struct GetJoinValue<T, std::void_t<decltype(std::declval<T>().value)> > {
    using type = decltype(std::declval<T>().value);
    static const type& Get(const T &obj) { return obj.value; }
};

struct DefaultKeySelector {
    template<typename T>
    decltype(auto) operator()(const T& val) const {
        return val.key;
    }
};

template<typename LeftRange, typename RightRange, typename LeftKeySelector, typename RightKeySelector>
class JoinAdapter {
public:
    using LeftValue = typename LeftRange::value_type;
    using RightValue = typename RightRange::value_type;
    using KeyT = decltype(std::declval<LeftKeySelector>()(std::declval<LeftValue>()));

    using LeftResultType = typename GetJoinValue<LeftValue>::type;
    using RightResultType = typename GetJoinValue<RightValue>::type;

    using value_type = JoinResult<LeftResultType, RightResultType>;

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = JoinResult<LeftResultType, RightResultType>;
        using reference = const value_type&;
        using pointer = const value_type*;
        using difference_type = std::ptrdiff_t;

        Iterator(typename LeftRange::iterator left_it,
                 typename LeftRange::iterator left_end,
                 const std::map<KeyT, RightResultType> &right_map,
                 LeftKeySelector left_key_selector)
            : left_it_(left_it), left_end_(left_end),
              right_map_(right_map), left_key_selector_(std::move(left_key_selector)) {
            if (left_it_ != left_end_)
                UpdateCurrent();
        }

        reference operator*() const {
            return current_;
        }

        pointer operator->() const {
            return &current_;
        }

        Iterator &operator++() {
            ++left_it_;
            UpdateCurrent();
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator &other) const {
            return left_it_ == other.left_it_;
        }

        bool operator!=(const Iterator &other) const {
            return left_it_ != other.left_it_;
        }

    private:
        typename LeftRange::iterator left_it_;
        typename LeftRange::iterator left_end_;
        const std::map<KeyT, RightResultType> &right_map_;
        LeftKeySelector left_key_selector_;
        value_type current_;

        void UpdateCurrent() {
            const LeftValue &left_elem = *left_it_;
            KeyT key = left_key_selector_(left_elem);
            auto it = right_map_.find(key);
            if (it == right_map_.end()) {
                current_ = JoinResult<LeftResultType, RightResultType>{
                    GetJoinValue<LeftValue>::Get(left_elem),
                    std::optional<RightResultType>{}
                };
            } else {
                current_ = JoinResult<LeftResultType, RightResultType>{
                    GetJoinValue<LeftValue>::Get(left_elem),
                    it->second
                };
            }
        }
    };

    JoinAdapter(LeftRange left_range, RightRange right_range,
                LeftKeySelector left_key_selector,
                RightKeySelector right_key_selector)
        : left_range_(std::move(left_range)), left_key_selector_(std::move(left_key_selector)) {
        for (const auto &right_elem: right_range) {
            KeyT key = right_key_selector(right_elem);
            right_map_.insert({key, GetJoinValue<RightValue>::Get(right_elem)});
        }
    }

    Iterator begin() {
        return Iterator(left_range_.begin(), left_range_.end(), right_map_, left_key_selector_);
    }

    Iterator end() {
        return Iterator(left_range_.end(), left_range_.end(), right_map_, left_key_selector_);
    }

private:
    LeftRange left_range_;
    std::map<KeyT, RightResultType> right_map_;
    LeftKeySelector left_key_selector_;
};

template<typename RightRange, typename LeftKeySelector, typename RightKeySelector>
inline auto Join(RightRange right_range, LeftKeySelector left_key_selector, RightKeySelector right_key_selector) {
    return [=](auto left_range) {
        using LeftRangeType = decltype(left_range);
        return JoinAdapter<LeftRangeType, RightRange, LeftKeySelector, RightKeySelector>(
            std::move(left_range),
            std::move(right_range),
            left_key_selector,
            right_key_selector
        );
    };
}

template<typename RightRange>
inline auto Join(RightRange right_range) {
    return [=](auto left_range) {
        using LeftRangeType = decltype(left_range);
        return JoinAdapter<LeftRangeType, RightRange, DefaultKeySelector, DefaultKeySelector>(
            std::move(left_range),
            std::move(right_range),
            DefaultKeySelector{},
            DefaultKeySelector{}
        );
    };
}
