#include "chat_core/chat_store.hpp"
#include "chat_core/command.hpp"
#include "chat_core/errors.hpp"
#include "chat_core/message.hpp"
#include "chat_core/overloaded.hpp"
#include "chat_core/parser.hpp"
#include "chat_core/reaction_message.hpp"
#include "chat_core/system_message.hpp"
#include "chat_core/text_message.hpp"
#include "chat_core/user.hpp"
#include "chat_core/user_registry.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <variant>

using namespace chat;

int main() {
    ChatStore store;
    UserRegistry registry;

    store.create_room("general");
    store.create_room("random");

    std::string current_room = "general";
    std::shared_ptr<User> current_user; 

    std::cout << "Chat CLI (stage 04). Commands:\n"
              << "  /join <room>           — создать/войти в комнату\n"
              << "  /leave <room>          — покинуть комнату\n"
              << "  /msg <room> <text>     — отправить сообщение\n"
              << "  /msg #r1 #r2 <text>    — рассылка в несколько комнат\n"
              << "  /who                   — список комнат\n"
              << "  /history <room>        — история сообщений\n"
              << "  /type <room> <index>   — тип тела сообщения\n"
              << "  /react <room> <index>  — увеличить счётчик реакции\n"
              << "  /reaction <room> <emoji> — создать реакцию\n"
              << "  /nick <nickname>       — создать пользователя\n"
              << "  /participants <room>   — посмотреть пользователей\n"
              << "  /quit                  — выход\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        if (line == "/quit") break;

        try {
            if (auto cmd = parse(line); cmd) {
                std::visit(overloaded{
                    [&](const JoinCommand& c) {
                        store.create_room(std::string(c.room));
                        current_room = std::string(c.room);
                        
                        if (current_user) {
                            store.add_participant(current_room, current_user);
                        }
                        
                        std::cout << "Joined " << c.room << "\n";
                    },

                    [&](const LeaveCommand& c) {
                        std::cout << "Left " << c.room << "\n";
                    },

                    [&](const MsgCommand& c) {
                        store.post(std::string(c.room), Message{
                            "user",
                            std::make_unique<TextMessage>(std::string(c.text)),
                            {}
                        });
                        std::cout << "Sent to " << c.room << "\n";
                    },

                    [&](const BroadcastCommand& c) {
                        std::vector<std::string> rooms;
                        rooms.reserve(c.rooms.size());
                        for (auto sv : c.rooms) {
                            rooms.emplace_back(sv);
                        }
                        store.broadcast(rooms, Message{
                            "user",
                            std::make_unique<TextMessage>(std::string(c.text)),
                            {}
                        });
                        std::cout << "Broadcast to " << c.rooms.size() << " rooms\n";
                    },

                    [&](const WhoCommand&) {
                        store.for_each_room(
                            [](const std::string& room,
                               const std::vector<Message>& msgs) {
                                std::cout << "  " << room
                                          << " (" << msgs.size() << " msgs)\n";
                            });
                    },

                    [&](const HistoryCommand& c) {
                        if (auto h = store.history(std::string(c.room)); h) {
                            for (const auto& msg : h->get()) {
                                std::cout << "  " << msg.author << ": "
                                          << msg.body->to_string() << "\n";
                            }
                        } else {
                            std::cout << "Room '" << c.room << "' not found\n";
                        }
                    },

                    [&](const TypeCommand& c) {
                        if (auto m = store.message_at(std::string(c.room), c.index); m) {
                            std::cout << m->get().body->type_name() << "\n";
                        } else {
                            std::cout << "Message not found\n";
                        }
                    },

                    [&](const ReactCommand& c) {
                        if (auto m = store.message_at(std::string(c.room), c.index); m) {
                            if (auto* reaction = dynamic_cast<ReactionMessage*>(m->get().body.get())) {
                                reaction->increment();
                                std::cout << "Reaction " << reaction->emoji()
                                          << " count: " << reaction->count() << "\n";
                            } else {
                                throw TypeMismatchError("Reaction", m->get().body->type_name());
                            }
                        } else {
                            std::cout << "Message not found\n";
                        }
                    },

                    [&](const ReactionCommand& c) {
                        store.post(std::string(c.room), Message{
                            "user",
                            std::make_unique<ReactionMessage>(std::string(c.emoji), 0),
                            {}
                        });
                        std::cout << "Created reaction " << c.emoji << " in " << c.room << "\n";
                    },

                    [&](const ParticipantsCommand& c) {
                        auto participants = store.get_active_participants(std::string(c.room));
                        if (participants.empty()) {
                            std::cout << "No active participants in " << c.room << "\n";
                        } else {
                            std::cout << "Participants in " << c.room << ":\n";
                            for (const auto& u : participants) {
                                std::cout << "  - " << u->nickname() << "\n";
                            }
                        }
                    },

                    [&](const NickCommand& c) {
                        current_user = registry.create_user(std::string(c.nickname));
                        store.add_participant(current_room, current_user);
                        std::cout << "Nickname set to '" << c.nickname 
                                << "' in room " << current_room << "\n";
                    },

                }, *cmd);
            } else {
                if (!line.empty()) {
                    std::cout << "Unknown command or bad syntax\n";
                }
            }
        }
        catch (const ChatError& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << "\n";
        }
        catch (...) {
            std::cerr << "Unknown error\n";
            throw;
        }
    }

    std::cout << "Bye!\n";
    return 0;
}