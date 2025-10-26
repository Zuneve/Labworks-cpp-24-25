#pragma once

#include "MyInvoke.h"


template<typename> class Function;

template<typename Ret, typename... Args>
class Function<Ret(Args...)> {
private:
    struct Base {
        virtual Ret Call(Args&&...) = 0;
        virtual ~Base() = default;
    };

    template<typename F>
    struct Derived : Base {
        F f;
        Derived(const F& fn) : f(fn) {}
        Derived(F&& fn) : f(std::move(fn)) {}
        Ret Call(Args&&... args) override {
            return MyInvoke(f, std::forward<Args>(args)...);
        }
    };

    Base* ptr = nullptr;

public:
    Function() = default;

    template<typename F>
    Function(F&& fn) {
        using Decayed = std::remove_reference_t<F>;
        ptr = new Derived<Decayed>(std::forward<F>(fn));
        //TODO small object optimization;
    }

    Function(Function&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    Function& operator=(Function&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    Function(const Function&) = delete;
    Function& operator=(const Function&) = delete;

    ~Function() {
        delete ptr;
    }

    Ret operator()(Args... args) const {
        return ptr->Call(std::forward<Args>(args)...);
    }
};
