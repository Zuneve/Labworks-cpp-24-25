#pragma once

#include <filesystem>

class Dir {
public:
    using value_type = std::filesystem::path;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;

    explicit Dir(const std::filesystem::path &dir, bool recursive = false)
        : dir_(dir), is_recursive_(recursive) {
        if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
            throw std::invalid_argument("Invalid directory path");
        }
    }

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::filesystem::path;
        using difference_type = std::ptrdiff_t;
        using reference = const value_type &;
        using pointer = const value_type *;

        explicit Iterator(const std::filesystem::path &dir, bool recursive = false, bool end = false)
            : dir_(dir), is_recursive_(recursive), end_(end) {
            if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
                throw std::invalid_argument("Invalid directory path");
            }
        }

        reference operator*() {
            initialize();
            return current_;
        }

        Iterator &operator++() {
            initialize();
            advance();
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator &other) const {
            if (end_ && other.end_) return true;
            if (end_ != other.end_ || is_recursive_ != other.is_recursive_) {
                return false;
            }
            if (is_recursive_) {
                return recursive_it_ == other.recursive_it_;
            }
            return it_ == other.it_;
        }

        bool operator!=(const Iterator &other) const {
            return !(*this == other);
        }

    private:
        std::filesystem::path dir_;
        bool is_recursive_;
        bool initialized_ = false;
        bool end_;
        value_type current_;

        std::filesystem::directory_iterator it_;
        std::filesystem::recursive_directory_iterator recursive_it_;

        void initialize() {
            if (!initialized_) {
                if (is_recursive_) {
                    recursive_it_ = std::filesystem::recursive_directory_iterator(dir_);
                } else {
                    it_ = std::filesystem::directory_iterator(dir_);
                }
                advance();
                initialized_ = true;
            }
        }

        void advance() {
            if (end_) return;
            if (is_recursive_) {
                if (recursive_it_ != std::filesystem::end(recursive_it_)) {
                    current_ = *recursive_it_;
                    ++recursive_it_;
                } else {
                    end_ = true;
                }
            } else {
                if (it_ != std::filesystem::end(it_)) {
                    current_ = *it_;
                    ++it_;
                } else {
                    end_ = true;
                }
            }
        }
    };

    using iterator = Iterator;

    iterator begin() const {
        return Iterator(dir_, is_recursive_);
    }

    iterator end() const {
        return Iterator(dir_, is_recursive_, true);
    }
private:
    std::filesystem::path dir_;
    bool is_recursive_ = false;
};
