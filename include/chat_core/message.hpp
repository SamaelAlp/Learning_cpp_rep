#pragma once

#include "chat_core/spy.hpp"

#include <chrono>
#include <string>
#include <utility>
#include <variant>

namespace chat {

using Timestamp = std::chrono::system_clock::time_point;

struct TextBody {
    std::string text;
};

struct SystemNotice {
    std::string notice;
};

struct Reaction {
    std::string emoji;
    int count = 0;
};

using MessageBody = std::variant<TextBody, SystemNotice, Reaction>;

struct Message {
    std::string author;
    MessageBody body;
    Timestamp   timestamp{};
    Spy         spy{};

    Message() = default;

    Message(std::string author_, std::string text_,
            Timestamp ts = std::chrono::system_clock::now())
        : author(std::move(author_))
        , body(TextBody{std::move(text_)})
        , timestamp(ts)
    {}

    Message(std::string author_, MessageBody body_,
            Timestamp ts = std::chrono::system_clock::now())
        : author(std::move(author_))
        , body(std::move(body_))
        , timestamp(ts)
    {}
};

}