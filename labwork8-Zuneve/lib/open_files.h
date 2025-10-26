#pragma once
#include <fstream>
#include <iterator>
#include <utility>
#include <type_traits>

template<typename Range>
class OpenFilesAdapter {
public:
    using value_type = std::ifstream &;

    class Iterator {
    public:
        using iterator_type = typename Range::iterator;
        using iterator_category = std::input_iterator_tag;
        using value_type = std::ifstream;
        using reference = std::ifstream &;
        using pointer = std::ifstream *;
        using difference_type = std::ptrdiff_t;

        Iterator(iterator_type current, iterator_type end)
            : current_(std::move(current)), end_(std::move(end)), stream_(new std::ifstream) {
        }

        Iterator(const Iterator &other)
            : current_(other.current_), end_(other.end_), stream_(new std::ifstream) {
        }

        Iterator(const iterator_type &other)
            : current_(other.current_), end_(other.end_), stream_(new std::ifstream) {
        }

        ~Iterator() {
            if (stream_->is_open()) {
                stream_->close();
            }
            delete stream_;
        }

        reference operator*() {
            if (!initialized_) {
                OpenCurrent();
                initialized_ = true;
            }
            return *stream_;
        }

        pointer operator->() {
            return stream_;
        }

        Iterator &operator++() {
            ++current_;
            OpenCurrent();
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator &other) const {
            return current_ == other.current_;
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

    private:
        bool initialized_ = false;
        iterator_type current_;
        iterator_type end_;
        std::ifstream *stream_;

        void OpenCurrent() {
            if (stream_->is_open()) {
                stream_->close();
            }
            if (current_ != end_) {
                stream_->open(*current_);
            }
        }
    };

    explicit OpenFilesAdapter(Range range)
        : range_(std::move(range)) {
    }

    Iterator begin() {
        return Iterator(range_.begin(), range_.end());
    }

    Iterator end() {
        return Iterator(range_.end(), range_.end());
    }

    Iterator begin() const {
        return Iterator(range_.begin(), range_.end());
    }

    Iterator end() const {
        return Iterator(range_.end(), range_.end());
    }

    using iterator = Iterator;

private:
    Range range_;
};

inline auto OpenFiles() {
    return [](auto &&range) {
        using RangeType = std::decay_t<decltype(range)>;
        return OpenFilesAdapter<RangeType>(std::forward<decltype(range)>(range));
    };
}
