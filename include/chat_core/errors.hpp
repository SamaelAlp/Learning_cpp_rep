#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <string_view>

namespace chat {

class ChatError : public std::runtime_error {
public:
    explicit ChatError(std::string msg) : std::runtime_error(std::move(msg)) {}
};

class ParseError : public ChatError {
public:
    explicit ParseError(std::string_view input)
        : ChatError("Parse error: " + std::string(input))
        , input_(input)
    {}

    [[nodiscard]] const std::string& input() const noexcept { return input_; }

private:
    std::string input_;
};

class UnknownRoomError : public ChatError {
public:
    explicit UnknownRoomError(std::string room)
        : ChatError("Unknown room: " + room)
        , room_(std::move(room))
    {}

    [[nodiscard]] const std::string& room() const noexcept { return room_; }

private:
    std::string room_;
};

class NicknameTakenError : public ChatError {
public:
    explicit NicknameTakenError(std::string nickname)
        : ChatError("Nickname taken: " + nickname)
        , nickname_(std::move(nickname))
    {}

    [[nodiscard]] const std::string& nickname() const noexcept { return nickname_; }

private:
    std::string nickname_;
};

class TypeMismatchError : public ChatError {
public:
    TypeMismatchError(std::string expected, std::string actual)
        : ChatError("Type mismatch: expected " + expected + ", got " + actual)
        , expected_(std::move(expected))
        , actual_(std::move(actual))
    {}

    [[nodiscard]] const std::string& expected() const noexcept { return expected_; }
    [[nodiscard]] const std::string& actual()   const noexcept { return actual_; }

private:
    std::string expected_;
    std::string actual_;
};

void print_exception_chain(const std::exception& e, int level = 0);

}