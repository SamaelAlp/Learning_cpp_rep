#pragma once

#include <string_view>
#include <vector>

namespace chat {

std::vector<std::string_view> tokenize(std::string_view input);

}