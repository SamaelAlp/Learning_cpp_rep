#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace chat {

struct JoinCommand        { std::string room; };
struct LeaveCommand       { std::string room; };

struct MsgCommand         { std::vector<std::string> rooms; std::string text; };

struct EmoteCommand       { std::string room; std::string emoji; };
struct WhoCommand         {};
struct HistoryCommand     { std::string room; };
struct TypeCommand        { std::string room; std::size_t index; };
struct ReactCommand       { std::string room; std::size_t index; };

struct NickCommand        { std::string nickname; };
struct ParticipantsCommand{ std::string room; };
struct ReactionCommand    { std::string room; std::string emoji; };
struct QuitCommand        {};

using Command = std::variant<
    JoinCommand, LeaveCommand, MsgCommand, EmoteCommand,
    WhoCommand, HistoryCommand, TypeCommand, ReactCommand,
    NickCommand, ParticipantsCommand, ReactionCommand, QuitCommand
>;

std::optional<Command> parse(std::string_view input);

}