#include "chat_core/chat_store.hpp"
#include "chat_core/command.hpp"
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

    std::cout << "Chat CLI (stage 02). Commands:\n"
              << "  /join <room>           — создать/войти в комнату\n"
              << "  /leave <room>          — покинуть комнату\n"
              << "  /msg <room> <text>     — отправить сообщение\n"
              << "  /who                   — список комнат\n"
              << "  /history <room>        — история сообщений\n"
              << "  /type <room> <index>   — тип тела сообщения\n"
              << "  /react <room> <index>  — увеличить счётчик реакции\n"
              << "  /quit                  — выход\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        if (line == "/quit") break;

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
                        for (const auto& [author, body, ts, spy] : h->get()) {
                            std::visit(overloaded{
                                [&](const TextBody& t) {
                                    std::cout << "  " << author
                                              << ": " << t.text << "\n";
                                },
                                [&](const SystemNotice& s) {
                                    std::cout << "  [system] "
                                              << s.notice << "\n";
                                },
                                [&](const Reaction& r) {
                                    std::cout << "  " << author
                                              << ": " << r.emoji
                                              << " x" << r.count << "\n";
                                },
                            }, body);
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
                            std::cout << "Error: message is not a Reaction\n";
                        }
                    } else {
                        std::cout << "Message not found\n";
                    }
                },

            }, *cmd);
        } else {
            if (!line.empty()) {
                std::cout << "Unknown command or bad syntax\n";
            }
        }
    }

    std::cout << "Bye!\n";
    return 0;
}