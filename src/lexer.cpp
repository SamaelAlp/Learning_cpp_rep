#include "chat_core/lexer.hpp"

#include <cctype>

namespace chat {

template <typename... Chars>
constexpr bool is_one_of(char c, Chars... chars) {
    return ((c == chars) || ...);
}

std::vector<std::string_view> tokenize(std::string_view input) {
    std::vector<std::string_view> tokens;
    std::size_t i = 0;
    const std::size_t n = input.size();

    while (i < n) {

        while (i < n &&
               is_one_of(input[i], ' ', '\t', '\n', '\r')) {
            ++i;
        }
        if (i >= n) break;

        if (input[i] == '"') {

            ++i;
            const auto start = i;
            while (i < n && input[i] != '"') ++i;
            tokens.push_back(input.substr(start, i - start));
            if (i < n) ++i;
        } else {

            const auto start = i;
            while (i < n &&
                   !is_one_of(input[i], ' ', '\t', '\n', '\r')) {
                ++i;
            }
            tokens.push_back(input.substr(start, i - start));
        }
    }

    return tokens;
}

}