#pragma once

#include "chat_core/imessage.hpp"
#include "chat_core/spy.hpp"
#include "chat_core/text_message.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <utility>

namespace chat {

using Timestamp = std::chrono::system_clock::time_point;

struct Message {
    std::string author;
    std::unique_ptr<IMessage> body;
    Timestamp timestamp{};
    Spy spy{};

    Message() = default;

    Message(std::string author_, std::string text,
            Timestamp ts = std::chrono::system_clock::now())
        : author(std::move(author_))
        , body(std::make_unique<TextMessage>(std::move(text)))
        , timestamp(ts)
    {}

    Message(std::string author_, std::unique_ptr<IMessage> body_,
            Timestamp ts = std::chrono::system_clock::now())
        : author(std::move(author_))
        , body(std::move(body_))
        , timestamp(ts)
    {}

    Message(const Message& other)
        : author(other.author)
        , body(other.body ? other.body->clone() : nullptr)
        , timestamp(other.timestamp)
        , spy(other.spy)
    {}

    Message& operator=(const Message& other) {
        if (this != &other) {
            author = other.author;
            body = other.body ? other.body->clone() : nullptr;
            timestamp = other.timestamp;
            spy = other.spy;
        }
        return *this;
    }

    Message(Message&&) noexcept = default;
    Message& operator=(Message&&) noexcept = default;

    ~Message() = default;
};

}