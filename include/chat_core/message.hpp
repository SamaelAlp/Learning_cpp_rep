#pragma once

#include "chat_core/spy.hpp"

#include <chrono>
#include <string>
#include <utility>

namespace chat {

using Timestamp = std::chrono::system_clock::time_point;

struct Message {
    std::string author;
    std::string text;
    Timestamp   timestamp{};
    Spy         spy{};

    Message() = default;

    Message(std::string author_, std::string text_, Timestamp ts = std::chrono::system_clock::now())
        : author(std::move(author_))
        , text(std::move(text_))
        , timestamp(ts)
    {}
};

}