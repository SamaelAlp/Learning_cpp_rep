#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>

namespace chat {

template <typename T>
concept Serializable = requires(T obj, const std::vector<std::byte> bytes) {
    { obj.to_bytes() } -> std::convertible_to<std::vector<std::byte>>;
    { T::from_bytes(bytes) } -> std::same_as<T>;
};

template <typename T>
concept NickLike = requires(T t) {
    { std::string_view(t) } -> std::convertible_to<std::string_view>;
    requires std::equality_comparable<T>;
};

}