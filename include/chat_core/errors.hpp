#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

namespace chat {

class ChatError : public std::runtime_error {
public:
    explicit ChatError(const std::string& message)
        : std::runtime_error(message)
    {}
};

class ParseError : public ChatError {
public:
    explicit ParseError(const std::string& message)
        : ChatError("Parse error: " + message)
    {}
};

class UnknownRoomError : public ChatError {
public:
    explicit UnknownRoomError(std::string_view room)
        : ChatError("Unknown room: " + std::string(room))
        , room_(room)
    {}

    [[nodiscard]] const std::string& room() const noexcept {
        return room_;
    }

private:
    std::string room_;
};

class NicknameTakenError : public ChatError {
public:
    explicit NicknameTakenError(std::string_view nickname)
        : ChatError("Nickname already taken: " + std::string(nickname))
        , nickname_(nickname)
    {}

    [[nodiscard]] const std::string& nickname() const noexcept {
        return nickname_;
    }

private:
    std::string nickname_;
};

class TypeMismatchError : public ChatError {
public:
    TypeMismatchError(std::string_view expected, std::string_view actual)
        : ChatError("Type mismatch: expected " + std::string(expected) +
                    ", got " + std::string(actual))
        , expected_(expected)
        , actual_(actual)
    {}

    [[nodiscard]] const std::string& expected() const noexcept {
        return expected_;
    }

    [[nodiscard]] const std::string& actual() const noexcept {
        return actual_;
    }

private:
    std::string expected_;
    std::string actual_;
};

}