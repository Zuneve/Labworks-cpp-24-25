#pragma once

#include <iostream>

template <typename Range, typename Func>
class TransformAdapter {
public:

    using value_type = decltype(std::declval<Func>()
        (std::declval<typename Range::value_type&>()));

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using iterator_type = typename Range::iterator;
        using value_type = typename TransformAdapter::value_type;
        using reference = value_type&;
        using difference_type = std::ptrdiff_t;

        Iterator(iterator_type it, Func func)
                    : it_(std::move(it)), func_(std::move(func)) {}

        Iterator& operator=(const Iterator& other) noexcept {
            it_ = other.it_;
            func_ = other.func_;
            return *this;
        }

        value_type operator*() {
            return func_(*it_);
        }

        Iterator& operator++() {
            ++it_;
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }

        bool operator!=(const Iterator& other) const {
            return it_ != other.it_;
        }

    private:
        typename Range::iterator it_;
        Func func_;
    };

    TransformAdapter(Range range, Func func)
        : range_(std::move(range)), func_(std::move(func)) {}

    Iterator begin() {
        return Iterator(range_.begin(), func_);
    }

    Iterator end() {
        return Iterator(range_.end(), func_);
    }

    Iterator begin() const {
        return Iterator(range_.begin(), func_);
    }

    Iterator end() const {
        return Iterator(range_.end(), func_);
    }

    using iterator = Iterator;
private:
    Range range_;
    Func func_;
};

template <typename Func>
inline auto Transform(Func func) {
    return [func = std::move(func)](auto&& range) {
        return TransformAdapter<std::decay_t<decltype(range)>,
            Func>(std::forward<decltype(range)>(range), func);
    };
}
