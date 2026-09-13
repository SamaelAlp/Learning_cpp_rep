#pragma once

#include <cstddef>
#include <string_view>
#include <variant>

namespace chat {

struct JoinCommand    { std::string_view room; };
struct LeaveCommand   { std::string_view room; };
struct MsgCommand     { std::string_view room; std::string_view text; };
struct WhoCommand     {};
struct HistoryCommand { std::string_view room; };
struct TypeCommand    { std::string_view room; std::size_t index; };
struct ReactCommand   { std::string_view room; std::size_t index; };

using Command = std::variant<
    JoinCommand,
    LeaveCommand,
    MsgCommand,
    WhoCommand,
    HistoryCommand,
    TypeCommand,
    ReactCommand
>;

}