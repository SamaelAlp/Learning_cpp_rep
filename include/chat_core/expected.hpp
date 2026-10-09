#pragma once

#include <variant>
#include <stdexcept>

namespace chat {

template <typename T, typename E>
class Expected {
public:
    // Конструктор для значения
    Expected(const T& value) : data_(value) {}
    Expected(T&& value) : data_(std::move(value)) {}

    struct error_tag {};
    Expected(error_tag, const E& error) : data_(error) {}
    Expected(error_tag, E&& error) : data_(std::move(error)) {}

    [[nodiscard]] bool has_value() const noexcept {
        return std::holds_alternative<T>(data_);
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] T& value() & {
        if (!has_value()) throw std::runtime_error("Expected has no value");
        return std::get<T>(data_);
    }

    [[nodiscard]] const T& value() const& {
        if (!has_value()) throw std::runtime_error("Expected has no value");
        return std::get<T>(data_);
    }

    [[nodiscard]] T&& value() && {
        if (!has_value()) throw std::runtime_error("Expected has no value");
        return std::get<T>(std::move(data_));
    }

    [[nodiscard]] E& error() & {
        if (has_value()) throw std::runtime_error("Expected has no error");
        return std::get<E>(data_);
    }

    [[nodiscard]] const E& error() const& {
        if (has_value()) throw std::runtime_error("Expected has no error");
        return std::get<E>(data_);
    }

    [[nodiscard]] T& operator*() & { return value(); }
    [[nodiscard]] const T& operator*() const& { return value(); }
    [[nodiscard]] T* operator->() { return &value(); }
    [[nodiscard]] const T* operator->() const { return &value(); }

    static Expected ok(const T& value) { return Expected(value); }
    static Expected ok(T&& value) { return Expected(std::move(value)); }
    static Expected fail(const E& error) { return Expected(error_tag{}, error); }
    static Expected fail(E&& error) { return Expected(error_tag{}, std::move(error)); }

private:
    std::variant<T, E> data_;
};

}