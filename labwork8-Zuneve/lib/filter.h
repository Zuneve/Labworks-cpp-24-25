#pragma once

template<typename Range, typename Predicate>
class FilterAdapter {
public:
    using value_type = typename Range::value_type;
    using iterator_type = typename Range::iterator;
    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = typename FilterAdapter::value_type;
        using difference_type = std::ptrdiff_t;

        Iterator(iterator_type current, iterator_type end, Predicate predicate)
            : current_(std::move(current)), end_(std::move(end)),
            predicate_(std::move(predicate)) {
            skip_invalid();
        }

        template <typename T>
        using iterator_t = decltype(std::begin(std::declval<T&>()));

        template <typename T>
        using reference_t = decltype(*std::declval<iterator_t<T>>());

        reference_t<Range> operator*() {
            return *current_;
        }

        Iterator& operator++() {
            ++current_;
            skip_invalid();
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp(*this);
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        iterator_type current_;
        iterator_type end_;
        Predicate predicate_;

        void skip_invalid() {
            while (current_ != end_ && !predicate_(*current_)) {
                ++current_;
            }
        }
    };

    FilterAdapter(Range range, Predicate pred)
        : range_(std::move(range)), predicate_(std::move(pred)) {
    }

    Iterator begin() {
        return Iterator(range_.begin(), range_.end(), predicate_);
    }

    Iterator end() {
        return Iterator(range_.end(), range_.end(), predicate_);
    }

    Iterator begin() const {
        return Iterator(range_.begin(), range_.end(), predicate_);
    }

    Iterator end() const {
        return Iterator(range_.end(), range_.end(), predicate_);
    }

    using iterator = Iterator;
private:
    Range range_;
    Predicate predicate_;
};

template <typename Predicate>
inline auto Filter(const Predicate& predicate) {
    return [predicate](auto&& range) {
        using RangeType = std::decay_t<decltype(range)>;
        return FilterAdapter<RangeType, Predicate>(range, predicate);
    };
}
