#pragma once

#include "ichat_store.hpp"

#include <memory>
#include <string>

namespace chat {

std::unique_ptr<IChatStore> make_engine(const std::string& name);

}