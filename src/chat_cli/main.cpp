#include "chat_core/chat_store.hpp"
#include "chat_core/command_registry.hpp"
#include "chat_core/convert.hpp"
#include "chat_core/i_chat_store.hpp"
#include "chat_core/lexer.hpp"
#include "chat_core/message.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

using namespace chat;

int main(int argc, char* argv[]) {
    std::string engine_name = "std";
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--engine") == 0 && i + 1 < argc) {
            engine_name = argv[++i];
        } else if (std::strncmp(argv[i], "--engine=", 9) == 0) {
            engine_name = argv[i] + 9;
        }
    }
    
    std::unique_ptr<IChatStore> store;
    try {
        store = make_engine(engine_name);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "Using engine: " << engine_name << "\n";
    
    CommandRegistry registry;
    store->create_room("general");
    store->create_room("random");
    
    std::string current_user;
    

    // ── Регистрация команд ────────────────────────────────────
    
    registry.register_command("/help", [&]() -> std::string {
        return "Commands:\n"
               "  /nick <nickname>          — set your nickname\n"
               "  /join <room>              — join/create a room\n"
               "  /leave <room>             — leave a room\n"
               "  /msg <room> <text>        — send a message\n"
               "  /reaction <room> <emoji>  — post a reaction\n"
               "  /who                      — list all rooms\n"
               "  /broadcast <text>         — send to all rooms\n"
               "  /history <room>           — show room history\n"
               "  /type <room> <index>      — show message type\n"
               "  /react <room> <index>     — increment reaction counter\n"
               "  /participants <room>      — show room participants\n"
               "  /kick <room> <nickname>   — kick user from room\n"
               "  /rooms                    — list rooms with sizes\n"
               "  /quit                     — exit\n";
    });
    
    registry.register_command("/nick", [&](std::string nickname) -> std::string {
        auto user = store->create_user(nickname);
        current_user = user->nickname();
        return "Nickname set: " + current_user;
    });
    
    registry.register_command("/join", [&](RoomName room) -> std::string {
        store->create_room(room.value);
        auto* r = store->room(room.value);
        if (r && !current_user.empty()) {
            auto user = store->find_user(current_user);
            if (user) r->add_participant(user);
        }
        return "Joined " + room.value;
    });
    
    registry.register_command("/leave", [&](RoomName room) -> std::string {

        return "Left " + room.value;
    });
    
    registry.register_command("/msg", [&](RoomName room, std::string text) -> std::string {
        auto* r = store->room(room.value);
        if (!r) return "Room not found: " + room.value;
        
        r->add_message(make_text_message(
            MessageId{static_cast<uint64_t>(r->messages().size()),
                      std::chrono::system_clock::now(), current_user},
            current_user, text
        ));
        return "Sent to " + room.value;
    });
    
    registry.register_command("/reaction", [&](RoomName room, std::string emoji) -> std::string {
        auto* r = store->room(room.value);
        if (!r) return "Room not found: " + room.value;
        
        r->add_message(make_reaction_message(
            MessageId{static_cast<uint64_t>(r->messages().size()),
                      std::chrono::system_clock::now(), current_user},
            current_user, emoji, 1
        ));
        return "Reaction " + emoji + " added";
    });
    
    registry.register_command("/who", [&]() -> std::string {
        std::string result = "Rooms:\n";
        store->for_each_room_sorted([&](const std::string& name, [[maybe_unused]] const Room& r) {
            result += "  " + name + "\n";
        });
        return result;
    });
    
    registry.register_command("/broadcast", [&](std::string text) -> std::string {
        int count = 0;
        store->for_each_room_mut([&]([[maybe_unused]] const std::string& name, Room& r) {
            r.add_message(make_text_message(
                MessageId{static_cast<uint64_t>(r.messages().size()),
                          std::chrono::system_clock::now(), current_user},
                current_user, text
            ));
            ++count;
        });
        return "Broadcasted to " + std::to_string(count) + " room(s)";
    });
    
    registry.register_command("/history", [&](RoomName room) -> std::string {
        auto* r = store->room(room.value);
        if (!r) return "Room not found: " + room.value;
        
        std::string result;
        for (const auto& msg : r->messages()) {
            result += "  " + msg->author() + ": " + msg->render() + "\n";
        }
        return result.empty() ? "No messages" : result;
    });
    
    registry.register_command("/type", [&](RoomName room, std::size_t index) -> std::string {
        auto* r = store->room(room.value);
        if (!r) return "Room not found: " + room.value;
        if (index >= r->messages().size()) return "Index out of range";
        
        return "Type: " + r->messages()[index]->type_name();
    });
    
    registry.register_command("/react", [&](RoomName room, std::size_t index) -> std::string {
        auto* r = store->room(room.value);
        if (!r) return "Room not found: " + room.value;
        if (index >= r->messages().size()) return "Index out of range";
        
        auto* msg = r->message_at(index);
        if (msg && msg->try_react()) {
            return msg->render();
        }
        return "Not a reaction";
    });
    
    registry.register_command("/participants", [&](RoomName room) -> std::string {
        auto* r = store->room(room.value);
        if (!r) return "Room not found: " + room.value;
        return "Participants: " + std::to_string(r->participant_count());
    });
    
    registry.register_command("/kick", [&](RoomName room, std::string nickname) -> std::string {
        return "Kicked " + nickname + " from " + room.value;
    });
    
    registry.register_command("/rooms", [&]() -> std::string {
        std::string result = "Available rooms:\n";
        store->for_each_room_sorted([&](const std::string& name, const Room& r) {
            result += "  " + name + " (" + std::to_string(r.messages().size()) + " messages)\n";
        });
        return result;
    });
    
    std::cout << "Chat CLI (stage 08). Type /help for commands.\n\n";
    
    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        
        if (line == "/quit" || line == "/exit") break;

        auto tokens = tokenize(line);
        std::string result = registry.execute(line, tokens);
        if (!result.empty()) {
            std::cout << result << "\n";
        }
    }
    
    return 0;
}