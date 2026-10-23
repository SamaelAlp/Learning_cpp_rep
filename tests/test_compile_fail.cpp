#include "chat_core/command_registry.hpp"
#include <vector>

using namespace chat;

int main() {
    CommandRegistry registry;
    
    registry.register_command("/bad", [](std::vector<int> v) -> std::string {
        return "bad";
    });
    
    return 0;
}