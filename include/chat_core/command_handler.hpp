#pragma once

#include "chat_core/function_traits.hpp"

#include <concepts>
#include <string>
#include <type_traits>

namespace chat {

template <typename F>
concept CommandHandler = requires {
    typename function_traits<std::decay_t<F>>::result_type;
    typename function_traits<std::decay_t<F>>::args_tuple;
} && std::is_convertible_v<
    typename function_traits<std::decay_t<F>>::result_type, 
    std::string
>;

}