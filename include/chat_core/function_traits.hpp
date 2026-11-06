#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace chat {

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
    using result_type = R;
    static constexpr std::size_t arity = sizeof...(Args);
    
    template <std::size_t N>
    using arg_type = std::tuple_element_t<N, std::tuple<Args...>>;
    
    using args_tuple = std::tuple<Args...>;
};

template <typename R, typename... Args>
struct function_traits<R(Args...)> : function_traits<R(*)(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R(*)(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(*)(Args...)> {};

template <typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

template <typename T>
struct function_traits<T&> : function_traits<std::decay_t<T>> {};

template <typename T>
struct function_traits<T&&> : function_traits<std::decay_t<T>> {};

template <typename T>
struct function_traits<const T> : function_traits<T> {};

template <typename T>
struct function_traits<volatile T> : function_traits<T> {};

}