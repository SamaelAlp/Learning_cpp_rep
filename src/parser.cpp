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

    return std::nullopt;
}

}