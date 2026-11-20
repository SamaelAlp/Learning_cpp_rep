#pragma once

#include "chat_core/command.hpp"
#include "chat_core/errors.hpp"
#include "chat_core/expected.hpp"

#include <optional>
#include <string_view>

namespace chat {

std::optional<Command> parse(std::string_view input);

Expected<Command, ParseError> parse_expected(std::string_view input);

}