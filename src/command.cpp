#include "chat_core/command.hpp"
#include "chat_core/errors.hpp"
#include "chat_core/lexer.hpp"

#include <charconv>
#include <string>
#include <string_view>

namespace chat {

namespace {

bool parse_index(std::string_view sv, std::size_t& out) {
    if (sv.empty()) return false;
    out = 0;
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), out);
    return ec == std::errc{} && ptr == sv.data() + sv.size();
}

std::string normalize_room_name(std::string_view sv) {
    if (!sv.empty() && sv[0] == '#') {
        sv.remove_prefix(1);
    }
    return std::string(sv);
}

std::string_view remainder_after(std::string_view input,
                                 const std::vector<std::string_view>& tokens,
                                 std::size_t token_idx) {
    const char* end = tokens[token_idx].data() + tokens[token_idx].size();
    const char* input_end = input.data() + input.size();
    while (end < input_end && *end == ' ') ++end;
    return std::string_view(end, static_cast<std::size_t>(input_end - end));
}

bool is_room_like(std::string_view sv) {
    return !sv.empty() && (sv[0] == '#' || sv[0] == '"');
}

MsgCommand parse_msg(std::string_view input,
                     const std::vector<std::string_view>& tokens) {
    std::vector<std::string> rooms;
    std::size_t i = 1;

    while (i < tokens.size() && is_room_like(tokens[i])) {
        rooms.push_back(normalize_room_name(tokens[i]));
        ++i;
    }

    if (rooms.empty()) {
        throw ParseError(input);
    }
    if (i >= tokens.size()) {
        throw ParseError(input);
    }

    std::string text(remainder_after(input, tokens, i - 1));
    return MsgCommand{std::move(rooms), std::move(text)};
}

}

std::optional<Command> parse(std::string_view input) {
    auto tokens = tokenize(input);
    if (tokens.empty()) return std::nullopt;

    const auto cmd = tokens[0];

    if (cmd == "/quit" || cmd == "/exit") {
        return std::nullopt;
    }

    if (cmd == "/join") {
        if (tokens.size() < 2) throw ParseError(input);
        return JoinCommand{normalize_room_name(tokens[1])};
    }

    if (cmd == "/leave") {
        if (tokens.size() < 2) throw ParseError(input);
        return LeaveCommand{normalize_room_name(tokens[1])};
    }

    if (cmd == "/msg") {
        return parse_msg(input, tokens);
    }

    if (cmd == "/emote") {
        if (tokens.size() < 3) throw ParseError(input);
        return EmoteCommand{std::string(tokens[1]), std::string(tokens[2])};
    }

    if (cmd == "/who") {
        return WhoCommand{};
    }

    if (cmd == "/history") {
        if (tokens.size() < 2) throw ParseError(input);
        return HistoryCommand{normalize_room_name(tokens[1])}; 
    }

    if (cmd == "/type") {
        if (tokens.size() < 3) throw ParseError(input);
        std::size_t idx = 0;
        if (!parse_index(tokens[2], idx)) throw ParseError(input);
        return TypeCommand{normalize_room_name(tokens[1]), idx};
    }

    if (cmd == "/react") {
        if (tokens.size() < 3) throw ParseError(input);
        std::size_t idx = 0;
        if (!parse_index(tokens[2], idx)) throw ParseError(input);
        return ReactCommand{normalize_room_name(tokens[1]), idx};
    }

    if (cmd == "/nick") {
        if (tokens.size() < 2) throw ParseError(input);
        return NickCommand{std::string(tokens[1])};
    }

    if (cmd == "/participants") {
        if (tokens.size() < 2) throw ParseError(input);
        return ParticipantsCommand{normalize_room_name(tokens[1])};
    }

    if (cmd == "/reaction") {
        if (tokens.size() < 3) throw ParseError(input);
        return ReactionCommand{normalize_room_name(tokens[1]), std::string(tokens[2])};
    }

    throw ParseError(input);
}

}