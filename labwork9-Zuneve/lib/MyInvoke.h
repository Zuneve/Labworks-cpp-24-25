#pragma once

#include <iostream>

#include "MyIsSame.h"
#include "MyRemoveCV.h"
#include "MyRemoveReference.h"

namespace detail {
    template <class>
    constexpr bool is_reference_wrapper_v = false;

    template <class U>
    constexpr bool is_reference_wrapper_v<std::reference_wrapper<U>> = true;

    template <class T>
    using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;

    template <class C, class Pointed, class Object, class... Args>
    constexpr decltype(auto) invoke_memptr(Pointed C::*member, Object&& object,
                                           Args&&... args) {
        using object_t = remove_cvref_t<Object>;
        constexpr bool is_member_function = std::is_function_v<Pointed>;
        constexpr bool is_wrapped = is_reference_wrapper_v<object_t>;
        constexpr bool is_derived_object = is_same_v<C, object_t>
                                           || std::is_base_of_v<C, object_t>;

        if constexpr (is_member_function) {
            if constexpr (is_derived_object)
                return (std::forward<Object>(object).*member)
                        (std::forward<Args>(args)...);
            else if constexpr (is_wrapped)
                return (object.get().*member)(std::forward<Args>(args)...);
            else
                return ((*std::forward<Object>(object)).*member)
                        (std::forward<Args>(args)...);
        } else {
            static_assert(std::is_object_v<Pointed> && sizeof...(args) == 0);
            if constexpr (is_derived_object)
                return std::forward<Object>(object).*member;
            else if constexpr (is_wrapped)
                return object.get().*member;
            else
                return (*std::forward<Object>(object)).*member;
        }
    }
} // namespace detail

template <class F, class... Args>
constexpr decltype(auto) MyInvoke(F&& f, Args&&... args)
    noexcept(std::is_nothrow_invocable_v<F, Args...>) {
    if constexpr (std::is_member_pointer_v<detail::remove_cvref_t<F>>)
        return detail::invoke_memptr(f, std::forward<Args>(args)...);
    else
        return std::forward<F>(f)(std::forward<Args>(args)...);
}
