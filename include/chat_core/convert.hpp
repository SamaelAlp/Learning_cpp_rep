#pragma once

#include <charconv>
#include <concepts>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace chat {

struct RoomName {
    std::string value;
    
    explicit RoomName(std::string v) : value(std::move(v)) {}
};

template <typename T>
T convert(std::string_view sv) {
    using Decayed = std::decay_t<T>;
    
    if constexpr (std::is_same_v<Decayed, std::string_view>) {
        return sv;
    }
    else if constexpr (std::is_same_v<Decayed, std::string>) {
        return std::string(sv);
    }
    else if constexpr (std::is_same_v<Decayed, RoomName>) {
        if (sv.empty() || sv[0] != '#') {
            throw std::invalid_argument("Room name must start with '#'");
        }
        return RoomName(std::string(sv.substr(1)));
    }
    else if constexpr (std::is_arithmetic_v<Decayed>) {
        Decayed value{};
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
        if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
            throw std::invalid_argument("Invalid number");
        }
        return value;
    }
    else {
        static_assert(
            std::is_same_v<Decayed, void>,
            "Unsupported type for convert<T>. "
            "Supported: string_view, string, arithmetic types, RoomName."
        );
        return T{};
    }
}

template <typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string_view>, int> = 0>
T convert_legacy(std::string_view sv) {
    return sv;
}

template <typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, int> = 0>
T convert_legacy(std::string_view sv) {
    return std::string(sv);
}

template <typename T, std::enable_if_t<std::is_same_v<std::decay_t<T>, RoomName>, int> = 0>
T convert_legacy(std::string_view sv) {
    if (sv.empty() || sv[0] != '#') {
        throw std::invalid_argument("Room name must start with '#'");
    }
    return RoomName(std::string(sv.substr(1)));
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<std::decay_t<T>>, int> = 0>
T convert_legacy(std::string_view sv) {
    using Decayed = std::decay_t<T>;
    Decayed value{};
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (ec != std::errc{} || ptr != sv.data() + sv.size()) {
        throw std::invalid_argument("Invalid number");
    }
    return value;
}

} 