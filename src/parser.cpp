#include "chat_core/parser.hpp"
#include "chat_core/lexer.hpp"

#include <charconv>
#include <cctype>

namespace chat {

namespace {

std::optional<std::size_t> parse_index(std::string_view sv) {
    if (sv.empty()) return std::nullopt;
    std::size_t result = 0;
    const auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), result);
    if (ec != std::errc{} || ptr != sv.data() + sv.size())
        return std::nullopt;
    return result;
}

}

std::optional<Command> parse(std::string_view input) {
    const auto tokens = tokenize(input);
    if (tokens.empty()) return std::nullopt;

    const auto cmd = tokens[0];

    if (cmd == "/join") {
        if (tokens.size() < 2) return std::nullopt;
        return JoinCommand{tokens[1]};
    }

    if (cmd == "/leave") {
        if (tokens.size() < 2) return std::nullopt;
        return LeaveCommand{tokens[1]};
    }

    if (cmd == "/msg") {
        if (tokens.size() < 3) return std::nullopt;
        const auto text_start =
            static_cast<std::size_t>(tokens[2].data() - input.data());
        auto text = input.substr(text_start);
        while (!text.empty() &&
               std::isspace(static_cast<unsigned char>(text.back()))) {
            text.remove_suffix(1);
        }
        if (tokens.size() >= 4 && tokens[2].front() == '#') {
            std::vector<std::string_view> rooms;
            for (std::size_t i = 1; i < tokens.size() - 1; ++i) {
                rooms.push_back(tokens[i]);
            }
            return BroadcastCommand{std::move(rooms), text};
        }
        return MsgCommand{tokens[1], text};
    }

    if (cmd == "/who") {
        return WhoCommand{};
    }

    if (cmd == "/history") {
        if (tokens.size() < 2) return std::nullopt;
        return HistoryCommand{tokens[1]};
    }

    if (cmd == "/type") {
        if (tokens.size() < 3) return std::nullopt;
        if (auto idx = parse_index(tokens[2]); idx) {
            return TypeCommand{tokens[1], *idx};
        }
        return std::nullopt;
    }

    if (cmd == "/react") {
        if (tokens.size() < 3) return std::nullopt;
        if (auto idx = parse_index(tokens[2]); idx) {
            return ReactCommand{tokens[1], *idx};
        }
        return std::nullopt;
    }

    if (cmd == "/reaction") {
        if (tokens.size() < 3) return std::nullopt;
        return ReactionCommand{tokens[1], tokens[2]};
    }

    if (cmd == "/participants") {
        if (tokens.size() < 2) return std::nullopt;
        return ParticipantsCommand{tokens[1]};
    }

    if (cmd == "/nick") {
        if (tokens.size() < 2) return std::nullopt;
        return NickCommand{tokens[1]};
    }

    return std::nullopt;
}

Expected<Command, ParseError> parse_expected(std::string_view input) {
    const auto tokens = tokenize(input);
    if (tokens.empty()) {
        return Expected<Command, ParseError>::fail(
            ParseError("empty input"));
    }

    const auto cmd = tokens[0];

    if (cmd == "/join") {
        if (tokens.size() < 2) {
            return Expected<Command, ParseError>::fail(
                ParseError("/join requires room name"));
        }
        return Expected<Command, ParseError>::ok(JoinCommand{tokens[1]});
    }

    if (cmd == "/leave") {
        if (tokens.size() < 2) {
            return Expected<Command, ParseError>::fail(
                ParseError("/leave requires room name"));
        }
        return Expected<Command, ParseError>::ok(LeaveCommand{tokens[1]});
    }

    if (cmd == "/msg") {
        if (tokens.size() < 3) {
            return Expected<Command, ParseError>::fail(
                ParseError("/msg requires room and text"));
        }
        const auto text_start =
            static_cast<std::size_t>(tokens[2].data() - input.data());
        auto text = input.substr(text_start);
        while (!text.empty() &&
               std::isspace(static_cast<unsigned char>(text.back()))) {
            text.remove_suffix(1);
        }
        if (tokens.size() >= 4 && tokens[2].front() == '#') {
            std::vector<std::string_view> rooms;
            for (std::size_t i = 1; i < tokens.size() - 1; ++i) {
                rooms.push_back(tokens[i]);
            }
            return Expected<Command, ParseError>::ok(
                BroadcastCommand{std::move(rooms), text});
        }
        return Expected<Command, ParseError>::ok(
            MsgCommand{tokens[1], text});
    }

    if (cmd == "/who") {
        return Expected<Command, ParseError>::ok(WhoCommand{});
    }

    if (cmd == "/history") {
        if (tokens.size() < 2) {
            return Expected<Command, ParseError>::fail(
                ParseError("/history requires room name"));
        }
        return Expected<Command, ParseError>::ok(HistoryCommand{tokens[1]});
    }

    if (cmd == "/type") {
        if (tokens.size() < 3) {
            return Expected<Command, ParseError>::fail(
                ParseError("/type requires room and index"));
        }
        if (auto idx = parse_index(tokens[2]); idx) {
            return Expected<Command, ParseError>::ok(
                TypeCommand{tokens[1], *idx});
        }
        return Expected<Command, ParseError>::fail(
            ParseError("invalid index"));
    }

    if (cmd == "/react") {
        if (tokens.size() < 3) {
            return Expected<Command, ParseError>::fail(
                ParseError("/react requires room and index"));
        }
        if (auto idx = parse_index(tokens[2]); idx) {
            return Expected<Command, ParseError>::ok(
                ReactCommand{tokens[1], *idx});
        }
        return Expected<Command, ParseError>::fail(
            ParseError("invalid index"));
    }

    if (cmd == "/reaction") {
        if (tokens.size() < 3) {
            return Expected<Command, ParseError>::fail(
                ParseError("/reaction requires room and emoji"));
        }
        return Expected<Command, ParseError>::ok(
            ReactionCommand{tokens[1], tokens[2]});
    }

    return Expected<Command, ParseError>::fail(
        ParseError("unknown command"));
}

}