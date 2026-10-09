#include "chat_core/chat_store.hpp"
#include "chat_core/command.hpp"
#include "chat_core/errors.hpp"
#include "chat_core/message.hpp"
#include "chat_core/overloaded.hpp"
#include "chat_core/parser.hpp"
#include "chat_core/spy.hpp"

#include <iostream>
#include <string>
#include <variant>

using namespace chat;

int main() {
    ChatStore store;
    store.create_room("general");
    store.create_room("random");

    std::cout << "Chat CLI (stage 03). Commands:\n"
              << "  /join <room>           — создать/войти в комнату\n"
              << "  /leave <room>          — покинуть комнату\n"
              << "  /msg <room> <text>     — отправить сообщение\n"
              << "  /msg #r1 #r2 <text>    — рассылка в несколько комнат\n"
              << "  /who                   — список комнат\n"
              << "  /history <room>        — история сообщений\n"
              << "  /type <room> <index>   — тип тела сообщения\n"
              << "  /reaction <room> <emoji> — создать реакцию\n"
              << "  /react <room> <index>  — увеличить счётчик реакции\n"
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
                        store.post(std::string(c.room), Message{
                            "system",
                            SystemNotice{std::string(c.room) + ": room created"},
                            {}
                        });
                        std::cout << "Joined " << c.room << "\n";
                    },

                    [&](const LeaveCommand& c) {
                        std::cout << "Left " << c.room << "\n";
                    },

                    [&](const MsgCommand& c) {
                        store.post(std::string(c.room), Message{
                            "user",
                            TextBody{std::string(c.text)},
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
                            TextBody{std::string(c.text)},
                            {}
                        });
                        std::cout << "Broadcast to " << c.rooms.size()
                                  << " rooms\n";
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
                                std::visit(overloaded{
                                    [&](const TextBody& t) {
                                        std::cout << "  " << msg.author << ": " << t.text << "\n";
                                    },
                                    [&](const SystemNotice& s) {
                                        std::cout << "  [system] " << s.notice << "\n";
                                    },
                                    [&](const Reaction& r) {
                                        std::cout << "  " << msg.author << ": " << r.emoji
                                                << " x" << r.count << "\n";
                                    },
                                }, msg.body);
                            }
                        } else {
                            std::cout << "Room '" << c.room << "' not found\n";
                        }
                    },

                    [&](const TypeCommand& c) {
                        if (auto m = store.message_at(std::string(c.room),
                                                      c.index); m) {
                            std::visit(overloaded{
                                [](const TextBody&)    { std::cout << "TextBody\n"; },
                                [](const SystemNotice&){ std::cout << "SystemNotice\n"; },
                                [](const Reaction&)    { std::cout << "Reaction\n"; },
                            }, m->get().body);
                        } else {
                            std::cout << "Message not found\n";
                        }
                    },

                    [&](const ReactCommand& c) {
                        if (auto m = store.message_at(std::string(c.room),
                                                      c.index); m) {
                            if (auto* r = std::get_if<Reaction>(&m->get().body)) {
                                ++r->count;
                                std::cout << "Reaction " << r->emoji
                                          << " count: " << r->count << "\n";
                            } else {
                                throw TypeMismatchError("Reaction", "other");
                            }
                        } else {
                            std::cout << "Message not found\n";
                        }
                    },

                    [&](const ReactionCommand& c) {
                        store.post(std::string(c.room), Message{
                            "user",
                            Reaction{std::string(c.emoji), 0},
                            {}
                        });
                        std::cout << "Created reaction " << c.emoji << " in " << c.room << "\n";
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