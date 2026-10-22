#include "chat_core/chat_store.hpp"
#include "chat_core/command.hpp"
#include "chat_core/errors.hpp"
#include "chat_core/message.hpp"
#include "chat_core/overloaded.hpp"

#include <iostream>
#include <string>

using namespace chat;

int main() {
    ChatStore store;

    store.create_room("general");
    store.create_room("random");

    std::string line;
    std::string current_user;

    std::cout << "Chat CLI (stage 05). Commands:\n"
              << "  /nick <nickname>\n"
              << "  /join <room>\n"
              << "  /leave <room>\n"
              << "  /msg <room> [<room>...] <text>\n"
              << "  /reaction <room> <emoji>\n"
              << "  /who\n"
              << "  /history <room>\n"
              << "  /type <room> <index>\n"
              << "  /react <room> <index>\n"
              << "  /participants <room>\n"
              << "  /quit\n\n";

    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::optional<Command> cmd;
        try {
            cmd = parse(line);
        } catch (const ParseError& e) {
            std::cout << "Error: " << e.what() << "\n";
            continue;
        } catch (const ChatError& e) {
            std::cout << "Error: " << e.what() << "\n";
            continue;
        }

        if (!cmd) {
            break; // /quit
        }

        std::visit(overloaded{

            [&](const NickCommand& c) {
                auto user = store.create_user(c.nickname);
                current_user = user->nickname();
                std::cout << "Nickname set: " << current_user << "\n";
            },

            [&](const JoinCommand& c) {
                store.create_room(c.room);
                auto* r = store.room(c.room);
                if (r && !current_user.empty()) {
                    auto user = store.find_user(current_user);
                    if (user) r->add_participant(user);
                }
                std::cout << "Joined " << c.room << "\n";
            },

            [&](const LeaveCommand& c) {
                std::cout << "Left " << c.room << "\n";
            },

            [&](const MsgCommand& c) {
                for (const auto& room_name : c.rooms) {
                    auto* r = store.room(room_name);
                    if (r) {
                        r->add_message(make_text_message(
                            MessageId{static_cast<uint64_t>(r->messages().size()),
                                      std::chrono::system_clock::now(),
                                      current_user},
                            current_user,
                            c.text
                        ));
                    } else {
                        std::cout << "Room not found: " << room_name << "\n";
                    }
                }
                std::cout << "Sent to " << c.rooms.size() << " room(s).\n";
            },

            [&](const ReactionCommand& c) {
                auto* r = store.room(c.room);
                if (r) {
                    r->add_message(make_reaction_message(
                        MessageId{static_cast<uint64_t>(r->messages().size()),
                                  std::chrono::system_clock::now(),
                                  current_user},
                        current_user,
                        c.emoji,
                        1
                    ));
                    std::cout << "Reaction " << c.emoji << " added.\n";
                } else {
                    std::cout << "Room not found.\n";
                }
            },

            [&](const WhoCommand&) {
                store.for_each_room([](const std::string& name, const Room&) {
                    std::cout << "  " << name << "\n";
                });
            },

            [&](const HistoryCommand& c) {
                auto* r = store.room(c.room);
                if (r) {
                    for (const auto& msg : r->messages()) {
                        std::cout << "  " << msg->author() << ": " << msg->render() << "\n";
                    }
                } else {
                    std::cout << "Room not found.\n";
                }
            },

            [&](const TypeCommand& c) {
                auto* r = store.room(c.room);
                if (r && c.index < r->messages().size()) {
                    std::cout << "Type: " << r->messages()[c.index]->type_name() << "\n";
                } else {
                    std::cout << "Room or index not found.\n";
                }
            },

            [&](const ReactCommand& c) {
                auto* r = store.room(c.room);
                if (r && c.index < r->messages().size()) {
                    auto* msg = r->message_at(c.index);
                    if (msg && msg->try_react()) {
                        std::cout << msg->render() << "\n";
                    } else {
                        std::cout << "Not a reaction.\n";
                    }
                } else {
                    std::cout << "Room or index not found.\n";
                }
            },

            [&](const ParticipantsCommand& c) {
                auto* r = store.room(c.room);
                if (r) {
                    std::cout << "Participants: " << r->participant_count() << "\n";
                } else {
                    std::cout << "Room not found.\n";
                }
            },

            [&](const EmoteCommand& c) {
                auto* r = store.room(c.room);
                if (r) {
                    r->add_message(make_reaction_message(
                        MessageId{static_cast<uint64_t>(r->messages().size()),
                                  std::chrono::system_clock::now(),
                                  current_user},
                        current_user,
                        c.emoji,
                        1
                    ));
                }
            },

            [&](const QuitCommand&) {
                // Ничего не делаем — цикл завершится
            }

        }, *cmd);
    }

    return 0;
}