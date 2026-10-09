#pragma once

#include <cstddef>
#include <string_view>
#include <variant>
#include <vector>

namespace chat {

struct JoinCommand    { std::string_view room; };
struct LeaveCommand   { std::string_view room; };
struct MsgCommand     { std::string_view room; std::string_view text; };
struct WhoCommand     {};
struct HistoryCommand { std::string_view room; };
struct TypeCommand    { std::string_view room; std::size_t index; };
struct ReactCommand   { std::string_view room; std::size_t index; };
struct ReactionCommand { std::string_view room; std::string_view emoji; };
struct ParticipantsCommand { std::string_view room; };
struct NickCommand    { std::string_view nickname; };

struct BroadcastCommand {
    std::vector<std::string_view> rooms;
    std::string_view text;
};

using Command = std::variant<
    JoinCommand,
    LeaveCommand,
    MsgCommand,
    WhoCommand,
    HistoryCommand,
    TypeCommand,
    ReactCommand,
    BroadcastCommand,
    ReactionCommand,
    ParticipantsCommand,
    NickCommand
>;

}