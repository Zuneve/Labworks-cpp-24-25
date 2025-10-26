#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <algorithm>

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
    static_assert(NodeMaxSize > 0, "NodeMaxSize must be positive");

    struct Node {
        Node *prev;
        Node *next;
        alignas(T) unsigned char elements[sizeof(T) * NodeMaxSize];
        size_t size;

        Node() : size(0), prev(nullptr), next(nullptr) {
        }
    };

    Node fake_node_;
    using node_allocator_ = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    node_allocator_ allocator_;
    std::size_t list_size_;

    void FakeInit() {
        fake_node_.prev = fake_node_.next = &fake_node_;
    }
    Node *CreateNode() {
        Node *new_node = std::allocator_traits<node_allocator_>::allocate(allocator_, 1);
        new_node->prev = nullptr;
        new_node->next = nullptr;
        new_node->size = 0;
        return new_node;
    }

    void DestroyNode(Node *node) {
        auto start = reinterpret_cast<T*>(node->elements);

        for (size_t i = 0; i < node->size; ++i) {
            std::allocator_traits<node_allocator_>::destroy(allocator_, start + i);
        }

        std::allocator_traits<node_allocator_>::deallocate(allocator_, node, 1);
    }

    void ConstructElement(Node *node, size_t index, const T &value) {
        auto start = reinterpret_cast<T*>(node->elements);
        std::allocator_traits<node_allocator_>::construct(allocator_, start + index, value);
    }

    void DestroyElement(Node *node, size_t index) {
        auto start = reinterpret_cast<T *>(node->elements);
        std::allocator_traits<node_allocator_>::destroy(allocator_, start + index);
    }

    T &GetValue(Node *node, size_t index) {
        return *reinterpret_cast<T*>(node->elements + (index * sizeof(value_type)));
    }

    template<bool IsConst>
    struct BaseIterator {
        using pointer = T*;
        using reference = T&;
        using difference_type = std::ptrdiff_t;

        using iterator_category = std::bidirectional_iterator_tag;
        using node_type = std::conditional_t<IsConst, const Node*, Node*>;
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer_type = std::conditional_t<IsConst, const T*, T*>;
        using reference_type = std::conditional_t<IsConst, const T&, T&>;

        node_type node_;
        size_t pos_;

        explicit BaseIterator(node_type node, size_t pos) : node_(node), pos_(pos) {
        }

        reference_type operator*() const {
            auto data_ptr = reinterpret_cast<pointer_type>(node_->elements);
            return data_ptr[pos_];
        }

        pointer_type operator->() const {
            auto data_ptr = reinterpret_cast<pointer_type>(node_->elements);
            return &data_ptr[pos_];
        }

        BaseIterator &operator++() {
            ++pos_;
            if (pos_ == node_->size) {
                node_ = node_->next;
                pos_ = 0;
            }
            return *this;
        }

        BaseIterator operator++(int) {
            BaseIterator copy = *this;
            ++(*this);
            return copy;
        }

        BaseIterator &operator--() {
            if (pos_ == 0) {
                node_ = node_->prev;
                pos_ = node_->size - 1;
            } else {
                --pos_;
            }
            return *this;
        }

        BaseIterator operator--(int) {
            BaseIterator copy = *this;
            --(*this);
            return copy;
        }

        bool operator==(const BaseIterator &other) const {
            return node_ == other.node_ && pos_ == other.pos_;
        }

        bool operator!=(const BaseIterator &other) const {
            return !(*this == other);
        }
    };

public:
    using pointer = T*;
    using const_pointer = const T*;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;
    using size_type = size_t;

    using allocator_type = Allocator;
    using iterator = BaseIterator<false>;
    using const_iterator = BaseIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    unrolled_list() : allocator_(Allocator()), list_size_(0) {
        FakeInit();
    }

    explicit unrolled_list(const Allocator &alloc) : allocator_(alloc), list_size_(0) {
        FakeInit();
    }

    allocator_type get_allocator() const noexcept {
        return allocator_;
    }


    unrolled_list(const unrolled_list &other)
        : allocator_(std::allocator_traits<node_allocator_>::select_on_container_copy_construction(other.allocator_)),
          list_size_(0) {
        FakeInit();
        for (auto it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
    }

    unrolled_list(const unrolled_list &other, const allocator_type &alloc)
        : allocator_(alloc), list_size_(other.list_size_) {
        FakeInit();
        if (!other.list_size_) {
            return;
        }

        for (auto it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
    }

    unrolled_list(size_type size, const value_type &value)
        : allocator_(Allocator()), list_size_(0) {
        FakeInit();
        for (size_t i = 0; i < size; ++i) {
            push_back(value);
        }
    }

    template<typename InputIt, typename = std::enable_if_t<!std::is_integral<InputIt>::value> >
    unrolled_list(InputIt first, InputIt last, const Allocator &alloc = Allocator())
        : allocator_(alloc), list_size_(0) {
        FakeInit();
        try {
            for (; first != last; ++first) {
                push_back(*first);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    unrolled_list(std::initializer_list<value_type> init)
        : allocator_(Allocator()), list_size_(0) {
        FakeInit();
        for (const auto &elem: init) {
            push_back(elem);
        }
    }

    unrolled_list &operator=(std::initializer_list<value_type> init) {
        clear();
        for (const auto &elem: init) {
            push_back(elem);
        }
        return *this;
    }

    unrolled_list &operator=(const unrolled_list &other) {
        if (this != &other) {
            clear();
            list_size_ = other.list_size_;
            auto it1 = other.begin();
            while (it1 != other.end()) {
                push_back(*it1);
                ++it1;
            }
        }
        return *this;
    }

    ~unrolled_list() {
        clear();
    }

    void swap(unrolled_list &rhs) noexcept {
        std::swap(fake_node_, rhs.fake_node_);

        fake_node_.prev->next = &fake_node_;
        fake_node_.next->prev = &fake_node_;

        rhs.fake_node_.prev->next = &rhs.fake_node_;
        rhs.fake_node_.next->prev = &rhs.fake_node_;

        std::swap(list_size_, rhs.list_size_);
        if constexpr (std::allocator_traits<Allocator>::propagate_on_container_swap::value) {
            swap(allocator_, rhs.allocator_);
        }
    }

    bool operator==(const unrolled_list &other) const {
        if (this == &other) {
            return true;
        }
        if (list_size_ != other.list_size_) {
            return false;
        }

        auto it1 = begin();
        auto it2 = other.begin();

        while (it1 != end() && it2 != other.end()) {
            if (*it1 != *it2) {
                return false;
            }
            ++it1;
            ++it2;
        }

        return it1 == end() && it2 == other.end();
    }

    bool operator!=(const unrolled_list &other) const {
        return !(*this == other);
    }

    void clear() noexcept {
        if (fake_node_.next == &fake_node_) return;
        Node *current = fake_node_.next;
        while (current != &fake_node_) {
            Node *next = current->next;
            DestroyNode(current);
            current = next;
        }
        list_size_ = 0;
    }

    void push_back(const T &value) {
        Node *new_node;
        if (fake_node_.prev == &fake_node_) {
            new_node = CreateNode();
            fake_node_.prev = fake_node_.next = new_node;
            new_node->next = new_node->prev = &fake_node_;
        } else if (fake_node_.prev->size == NodeMaxSize) {
            new_node = CreateNode();
            fake_node_.prev->next = new_node;
            new_node->prev = fake_node_.prev;
            new_node->next = &fake_node_;
            fake_node_.prev = new_node;
        }

        ConstructElement(fake_node_.prev, fake_node_.prev->size, value);

        ++fake_node_.prev->size;
        ++list_size_;
    }

    void push_front(const T &value) {
        if (fake_node_.next == &fake_node_) {
            Node *new_node = CreateNode();
            fake_node_.prev = fake_node_.next = new_node;
            new_node->next = new_node->prev = &fake_node_;
        } else if (fake_node_.next->size == NodeMaxSize) {
            Node *new_node = CreateNode();
            new_node->next = fake_node_.next;
            new_node->prev = &fake_node_;
            fake_node_.next->prev = new_node;
            fake_node_.next = new_node;
        }

        Node *first_node = fake_node_.next;

        for (size_t i = first_node->size; i > 0; --i) {
            try {
                ConstructElement(first_node, i, GetValue(first_node, i - 1));
                DestroyElement(first_node, i - 1);
            } catch (...) {
                // return T-values back to their places
                for (size_t j = i + 1; j < first_node->size; ++j) {
                    ConstructElement(first_node, j - 1, GetValue(first_node, j));
                    DestroyElement(first_node, j);
                }
                if (first_node->size == 0) {
                    DestroyNode(first_node);
                }
                return;
            }
        }

        ConstructElement(first_node, 0, value);

        ++first_node->size;
        ++list_size_;
    }

    void pop_back() noexcept {
        if (list_size_ == 0) {
            return;
        }
        Node *tail = fake_node_.prev;
        size_t last_index = tail->size - 1;

        DestroyElement(tail, last_index);

        --tail->size;
        --list_size_;

        if (tail->size == 0) {
            if (fake_node_.next != fake_node_.prev) {
                tail->prev->next = &fake_node_;
                fake_node_.prev = tail->prev;
            } else {
                fake_node_.next = fake_node_.prev = &fake_node_;
            }
            DestroyNode(tail);
        }
    }

    void pop_front() noexcept {
        if (list_size_ == 0) {
            return;
        }
        Node *first_node = fake_node_.next;
        T first_value = GetValue(first_node, 0);

        DestroyElement(first_node, 0);

        for (size_t i = 1; i < first_node->size; ++i) {
            try {
                ConstructElement(first_node, i - 1, GetValue(first_node, i));
                DestroyElement(first_node, i);
            } catch (...) {
                for (size_t j = first_node->size - 1; j >= i + 1; --j) {
                    ConstructElement(first_node, j, GetValue(first_node, j - 1));
                    DestroyElement(first_node, j - 1);
                }
                ConstructElement(first_node, 0, first_value);
                return;
            }
        }

        --first_node->size;
        --list_size_;

        if (first_node->size == 0) {
            if (fake_node_.next != fake_node_.prev) {
                fake_node_.next = first_node->next;
                first_node->next->prev = &fake_node_;
            } else {
                fake_node_.next = fake_node_.prev = &fake_node_;
            }
            DestroyNode(first_node);
        }
    }

    template<typename Iterator>
    iterator erase(Iterator iter) noexcept {
        Node *cur = iter.node_;
        if (list_size_ == 0 || cur == nullptr || cur == &fake_node_) {
            return iter;
        }

        std::size_t pos = iter.pos_;
        T erase_value = GetValue(cur, pos);

        DestroyElement(cur, pos);

        for (size_t i = pos + 1; i < cur->size; ++i) {
            try {
                ConstructElement(cur, i - 1, GetValue(cur, i));
                DestroyElement(cur, i);
            } catch (...) {
                for (size_t j = cur->size - 1; j >= i + 1; --j) {
                    ConstructElement(cur, j, GetValue(cur, j - 1));
                    DestroyElement(cur, i);
                }
                ConstructElement(cur, pos, erase_value);
                return iter;
            }
        }

        --cur->size;
        --list_size_;

        if (cur->size == 0) {
            if (list_size_ == 0) {
                fake_node_.next = fake_node_.prev = &fake_node_;
            }
            if (cur == fake_node_.next) {
                fake_node_.next = cur->next;
                cur->next->prev = &fake_node_;

                iterator res(cur->next, 0);
                DestroyNode(cur);

                return res;
            }
            if (cur == fake_node_.prev) {
                fake_node_.prev = cur->prev;
                cur->prev->next = &fake_node_;

                DestroyNode(cur);
                return iterator(&fake_node_, 0);
            }

            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;

            iterator res(cur->next, 0);

            DestroyNode(cur);
            return res;
        }

        if (pos == NodeMaxSize) {
            return Iterator(cur->next, 0);
        }
        return Iterator(cur, pos);
    }

    template<typename Iterator>
    iterator erase(Iterator it1, Iterator it2) noexcept {
        Iterator res = it1;
        int n = std::distance(it1, it2);
        for (int i = 0; i < n; ++i) {
            res = erase(res);
        }
        return res;
    }

    template<typename Iterator>
    iterator insert(Iterator iter, const value_type &value) {
        Node *cur = iter.node_;
        if (list_size_ == 0 || cur == &fake_node_) {
            push_back(value);
            return begin();
        }
        if (cur == nullptr) {
            return iter;
        }

        std::size_t pos = iter.pos_;

        if (cur->size == NodeMaxSize) {
            Node *new_node = CreateNode();
            std::size_t split_index = NodeMaxSize / 2;

            for (std::size_t i = split_index; i < NodeMaxSize; ++i) {
                try {
                    ConstructElement(new_node, i - split_index, GetValue(cur, i));
                    DestroyElement(cur, i);
                } catch (...) {
                    for (size_t j = 0; j < new_node->size; ++j) {
                        DestroyElement(new_node, j);
                    }
                    DestroyNode(new_node);
                    return iter;
                }
                ++new_node->size;
            }

            cur->size = split_index;

            new_node->next = cur->next;
            cur->next->prev = new_node;
            cur->next = new_node;
            new_node->prev = cur;

            if (pos >= split_index) {
                pos -= split_index;
                cur = new_node;
            }
        }

        for (size_t i = cur->size; i > pos; --i) {
            ConstructElement(cur, i, GetValue(cur, i - 1));
            DestroyElement(cur, i - 1);
        }

        ConstructElement(cur, pos, value);

        ++cur->size;
        ++list_size_;

        return iterator(cur, pos);
    }

    template<typename Iterator>
    iterator insert(Iterator it, size_type n, const value_type &value) {
        Iterator res = it;
        for (size_type i = 0; i < n; ++i) {
            insert(it, value);
        }
        return res;
    }

    template<typename Iterator1, typename Iterator2,
        typename = std::enable_if_t<!std::is_integral<Iterator2>::value> >
    iterator insert(Iterator1 p, Iterator2 it1, Iterator2 it2) {
        auto Iterator = p;
        for (Iterator2 it = it1; it != it2; ++it) {
            insert(Iterator, *it);
            ++Iterator;
        }
        return p;
    }

    template<typename Iterator>
    iterator insert(Iterator p, const unrolled_list &other) {
        return insert(p, other.begin(), other.end());
    }

    template<typename Iterator>
    void assign(Iterator first, Iterator last) {
        clear();
        for (; first != last; ++first) {
            push_back(*first);
        }
    }

    void assign(const unrolled_list &list) {
        assign(list.begin(), list.end());
    }

    void assign(size_t n, const T& value) {
        clear();
        for (size_t i = 0; i < n; ++i) {
            push_back(value);
        }
    }

    iterator begin() {
        return iterator{fake_node_.next, 0};
    }

    iterator end() {
        return iterator{&fake_node_, fake_node_.size};
    }

    const_iterator begin() const {
        return const_iterator{fake_node_.next, 0};
    }

    const_iterator end() const {
        return const_iterator{&fake_node_, fake_node_.size};
    }

    const_iterator cbegin() const {
        return const_iterator{&fake_node_.next, 0};
    }

    const_iterator cend() const {
        return const_iterator{&fake_node_, fake_node_.size};
    }

    reverse_iterator rbegin() {
        return reverse_iterator(iterator{&fake_node_, fake_node_.size});
    }

    reverse_iterator rend() {
        return reverse_iterator(iterator{fake_node_.next, 0});
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(iterator{&fake_node_, fake_node_.size});
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(iterator{fake_node_.next, 0});
    }

    const_reverse_iterator crbegin() const {
        return ConstReverseIterator(end());
    }

    const_reverse_iterator crend() const {
        return ConstReverseIterator(begin());
    }

    size_t max_size() const {
        return std::allocator_traits<node_allocator_>::max_size(allocator_) * NodeMaxSize;
    }

    reference front() {
        return GetValue(fake_node_.next, 0);
    }

    const_reference front() const {
        return GetValue(fake_node_.next, 0);
    }

    reference back() {
        return GetValue(fake_node_.prev, fake_node_.prev->size - 1);
    }

    const_reference back() const {
        return GetValue(fake_node_.prev, fake_node_.prev->size - 1);
    }

    size_t size() const noexcept { return list_size_; }
    bool empty() const noexcept { return list_size_ == 0; }
};

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
void swap(unrolled_list<T, NodeMaxSize, Allocator>& lhs,
    unrolled_list<T, NodeMaxSize, Allocator>& rhs) noexcept {
    lhs.swap(rhs);
}