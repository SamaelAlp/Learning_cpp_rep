#pragma once

#include "chat_core/command.hpp"

#include <optional>
#include <string_view>

namespace chat {

std::optional<Command> parse(std::string_view input);

}