#include "chat_core/spy.hpp"

namespace chat {

std::size_t Spy::copy_ctor_count  = 0;
std::size_t Spy::move_ctor_count  = 0;
std::size_t Spy::copy_assign_count = 0;
std::size_t Spy::move_assign_count = 0;

}