#include "chat_core/chat_store.hpp"
#include "chat_core/command_registry.hpp"
#include "chat_core/convert.hpp"
#include "chat_core/lexer.hpp"
#include "chat_core/message.hpp"

#include <iostream>
#include <string>

using namespace chat;

int main() {
    ChatStore store;
    CommandRegistry registry;
    
    store.create_room("general");
    store.create_room("random");
    
    std::string current_user;
    
    
    registry.register_command("/nick", [&](std::string nickname) -> std::string {
        auto user = store.create_user(nickname);
        current_user = user->nickname();
        return "Nickname set: " + current_user;
    });
    
    registry.register_command("/join", [&](RoomName room) -> std::string {
        store.create_room(room.value);
        auto* r = store.room(room.value);
        if (r && !current_user.empty()) {
            auto user = store.find_user(current_user);
            if (user) r->add_participant(user);
        }
        return "Joined " + room.value;
    });
    
    registry.register_command("/leave", [&](RoomName room) -> std::string {
        return "Left " + room.value;
    });
    
    registry.register_command("/msg", [&](RoomName room, std::string text) -> std::string {
        auto* r = store.room(room.value);
        if (!r) return "Room not found: " + room.value;
        
        r->add_message(make_text_message(
            MessageId{static_cast<uint64_t>(r->messages().size()),
                      std::chrono::system_clock::now(),
                      current_user},
            current_user,
            text
        ));
        return "Sent to " + room.value;
    });
    
    registry.register_command("/reaction", [&](RoomName room, std::string emoji) -> std::string {
        auto* r = store.room(room.value);
        if (!r) return "Room not found: " + room.value;
        
        r->add_message(make_reaction_message(
            MessageId{static_cast<uint64_t>(r->messages().size()),
                      std::chrono::system_clock::now(),
                      current_user},
            current_user,
            emoji,
            1
        ));
        return "Reaction " + emoji + " added";
    });
    
    registry.register_command("/who", [&]() -> std::string {
        std::string result = "Rooms:\n";
        store.for_each_room([&](const std::string& name, const Room&) {
            result += "  " + name + "\n";
        });
        return result;
    });
    
    registry.register_command("/history", [&](RoomName room) -> std::string {
        auto* r = store.room(room.value);
        if (!r) return "Room not found: " + room.value;
        
        std::string result;
        for (const auto& msg : r->messages()) {
            result += "  " + msg->author() + ": " + msg->render() + "\n";
        }
        return result.empty() ? "No messages" : result;
    });
    
    registry.register_command("/type", [&](RoomName room, std::size_t index) -> std::string {
        auto* r = store.room(room.value);
        if (!r) return "Room not found: " + room.value;
        if (index >= r->messages().size()) return "Index out of range";
        
        return "Type: " + r->messages()[index]->type_name();
    });
    
    registry.register_command("/react", [&](RoomName room, std::size_t index) -> std::string {
        auto* r = store.room(room.value);
        if (!r) return "Room not found: " + room.value;
        if (index >= r->messages().size()) return "Index out of range";
        
        auto* msg = r->message_at(index);
        if (msg && msg->try_react()) {
            return msg->render();
        }
        return "Not a reaction";
    });
    
    registry.register_command("/participants", [&](RoomName room) -> std::string {
        auto* r = store.room(room.value);
        if (!r) return "Room not found: " + room.value;
        return "Participants: " + std::to_string(r->participant_count());
    });
    
    
    registry.register_command("/kick", [&](RoomName room, std::string nickname) -> std::string {
        // Заглушка: реальная логика кика не реализована
        return "Kicked " + nickname + " from " + room.value;
    });
    
    registry.register_command("/rooms", [&]() -> std::string {
        std::string result = "Available rooms:\n";
        store.for_each_room([&](const std::string& name, const Room& r) {
            result += "  " + name + " (" + std::to_string(r.messages().size()) + " messages)\n";
        });
        return result;
    });

    registry.register_command("/broadcast", [&](std::string text) -> std::string {
        int count = 0;
        
        store.for_each_room([&]([[maybe_unused]] const std::string& name, Room& r) {
            r.add_message(make_text_message(
                MessageId{
                    static_cast<uint64_t>(r.messages().size()), 
                    std::chrono::system_clock::now(), 
                    current_user
                },
                current_user, 
                text
            ));
            ++count;
        });
        
        return "Broadcasted to " + std::to_string(count) + " room(s)";
    });
    
    
    std::cout << "Chat CLI (stage 06). Type /help for commands.\n\n";
    
    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        
        if (line == "/quit" || line == "/exit") break;
        
        if (line == "/help") {
            std::cout << "Commands:\n"
                      << "  /nick <nickname>\n"
                      << "  /join <room>\n"
                      << "  /leave <room>\n"
                      << "  /msg <room> <text>\n"
                      << "  /reaction <room> <emoji>\n"
                      << "  /who\n"
                      << "  /broadcast\n"
                      << "  /history <room>\n"
                      << "  /type <room> <index>\n"
                      << "  /react <room> <index>\n"
                      << "  /participants <room>\n"
                      << "  /kick <room> <nickname>\n"
                      << "  /rooms\n"
                      << "  /quit\n\n";
            continue;
        }
        
        auto tokens = tokenize(line);
        std::string result = registry.execute(line, tokens);
        if (!result.empty()) {
            std::cout << result << "\n";
        }
    }
    
    return 0;
}