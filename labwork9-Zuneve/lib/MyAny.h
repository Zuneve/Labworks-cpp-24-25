#pragma once
#include <type_traits>
#include <stdexcept>

class Any {
public:
    struct Base {
        virtual Base* Clone() const = 0;
        virtual ~Base() = default;
    };

    template<typename T>
    struct Derived : Base {
        T value;
        Derived(const T& value) : value(value) {}
        Derived(T&& value) : value(std::move(value)) {}
        Base* Clone() const override { return new Derived(value); }
    };

    Any() = default;

    template<typename T>
    Any(const T& value) : ptr(new
        Derived<std::remove_cv_t<std::remove_reference_t<T>>>(value)
    ) {}

    Any(Any&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
    Any& operator=(Any&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    Any(const Any& other) : ptr(other.ptr ? other.ptr->Clone() : nullptr) {}

    Any& operator=(const Any& other) {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr ? other.ptr->Clone() : nullptr;
        }
        return *this;
    }

    ~Any() {
        delete ptr;
    }

    template<typename T>
    friend T any_cast(const Any& a);

private:
    Base* ptr = nullptr;
};

template<typename T>
T any_cast(const Any& a) {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    auto derived = dynamic_cast<Any::Derived<U>*>(a.ptr);
    if (!derived) {
        throw std::bad_cast();
    }
    return derived->value;
}
