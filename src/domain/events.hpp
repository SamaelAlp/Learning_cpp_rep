#pragma once

#include <string>
#include <variant>

namespace chat {

struct MessagePosted {
    std::string room;
    std::string author;
    std::string text;
};

struct UserJoined {
    std::string room;
    std::string nickname;
};

struct UserLeft {
    std::string room;
    std::string nickname;
};

using DomainEvent = std::variant<MessagePosted, UserJoined, UserLeft>;

} 