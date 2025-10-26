#pragma once

#include <vector>
#include <string>

template <typename Range>
class SplitAdapter {
public:
    using value_type = std::string;

    class Iterator {
    public:
        using iterator_type = typename Range::iterator;
        using iterator_category = std::input_iterator_tag;
        using value_type = typename SplitAdapter::value_type;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = const value_type*;
        using difference_type = std::ptrdiff_t;

        Iterator(iterator_type it, iterator_type end, std::string delimiter)
            : current_it_(std::move(it))
            , end_it_(std::move(end))
            , delimiters_(std::move(delimiter)) {
        }

        reference operator*() {
            if (!initialized_) {
                AdvanceToNextToken();
                initialized_ = true;
            }
            return current_token_;
        }

        pointer operator->() const {
            return &current_token_;
        }

        Iterator& operator++() {
            auto& stream = *current_it_;
            if (stream.peek() == EOF) {
                stream.clear();
                stream.seekg(0);
                ++current_it_;
            }
            AdvanceToNextToken();

            return *this;
        }

        bool operator==(const Iterator& other) const {
            if (current_it_ == end_it_ && other.current_it_ == other.end_it_)
                return true;
            return current_it_ == other.current_it_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        void AdvanceToNextToken() {
            current_token_.clear();
            while (current_it_ != end_it_) {
                auto& stream = *current_it_;
                int ch = stream.peek();
                while (ch != EOF) {
                    if (delimiters_.find(static_cast<char>(ch)) != std::string::npos) {
                        stream.get();
                        return;
                    }
                    current_token_.push_back(static_cast<char>(stream.get()));
                    ch = stream.peek();
                }
                if (!current_token_.empty()) {
                    return;
                }
                ++current_it_;
            }
        }

        typename Range::iterator current_it_;
        typename Range::iterator end_it_;
        std::string delimiters_;
        value_type current_token_;
        bool initialized_ = false;
    };

    SplitAdapter(const Range& range, std::string delimiters)
        : range_(std::move(range)), delimiters_(std::move(delimiters)) {}

    Iterator begin() {
        return Iterator(range_.begin(), range_.end(), delimiters_);
    }

    Iterator end() {
        return Iterator(range_.end(), range_.end(), delimiters_);
    }

    Iterator begin() const {
        return Iterator(range_.begin(), range_.end(), delimiters_);
    }

    Iterator end() const {
        return Iterator(range_.end(), range_.end(), delimiters_);
    }

    using iterator = Iterator;
private:
    Range range_;
    std::string delimiters_;
};

inline auto Split(const std::string& delimiters = "\n") {
    return [delimiters](auto&& range) {
        using RangeType = std::decay_t<decltype(range)>;
        return SplitAdapter<RangeType>(std::forward<decltype(range)>(range), delimiters);
    };
}
