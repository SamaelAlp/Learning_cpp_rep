#include "chat_service.hpp"
#include "command_registry.hpp"
#include "convert.hpp"
#include "fake_clock.hpp"
#include "kafka_event_publisher.hpp"
#include "real_clock.hpp"
#include "file_log_sink.hpp"
#include "null_log_sink.hpp"
#include "null_event_publisher.hpp"
#include "lexer.hpp"
#include "std_engine.hpp"
#include "hash_engine.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

using chat::infrastructure::KafkaEventPublisher;
using chat::StdEngine;
using chat::HashEngine;
using chat::RealClock;
using chat::FileLogSink;
using chat::NullEventPublisher;

using namespace chat;

int main(int argc, char* argv[]) {
    std::string engine_name = "std";
    std::string kafka_topic = "chat.events";
    bool disable_kafka = false;
    
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--engine") == 0 && i + 1 < argc) {
            engine_name = argv[++i];
        } else if (std::strncmp(argv[i], "--engine=", 9) == 0) {
            engine_name = argv[i] + 9;
        } else if (std::strcmp(argv[i], "--no-kafka") == 0) {
            disable_kafka = true;
        }
    }
    
    std::unique_ptr<IChatStore> store;
    if (engine_name == "hash") {
        store = std::make_unique<HashEngine>();
    } else {
        store = std::make_unique<StdEngine>();
    }
    
    RealClock clock;
    FileLogSink log("chat.log");
    
    std::unique_ptr<IEventPublisher> publisher;
    if (disable_kafka) {
        std::cout << "Kafka disabled (--no-kafka)\n";
        publisher = std::make_unique<NullEventPublisher>();
    } else {
        try {
            publisher = std::make_unique<KafkaEventPublisher>(kafka_topic);
            std::cout << "Kafka publisher: topic=" << kafka_topic << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Warning: Kafka unavailable (" << e.what()
                      << "), falling back to null publisher\n";
            publisher = std::make_unique<NullEventPublisher>();
        }
    }

    application::ChatService service(
        std::move(store), clock, log, std::move(publisher));
    
    service.store().create_room("general");
    service.store().create_room("random");
    
    CommandRegistry registry;
    
    registry.register_command("/help", [&]() -> std::string {
        return "Commands:\n"
               "  /nick <nickname>\n"
               "  /join <room>\n"
               "  /leave <room>\n"
               "  /msg <room> <text>\n"
               "  /broadcast <text>\n"
               "  /history <room>\n"
               "  /rooms\n"
               "  /quit\n";
    });
    
    registry.register_command("/nick", [&](std::string nickname) -> std::string {
        return service.set_nickname(nickname);
    });
    
    registry.register_command("/join", [&](RoomName room) -> std::string {
        return service.join_room(room.value);
    });
    
    registry.register_command("/leave", [&](RoomName room) -> std::string {
        return service.leave_room(room.value);
    });
    
    registry.register_command("/msg", [&](RoomName room, std::string text) -> std::string {
        return service.post_message(room.value, std::move(text));
    });
    
    registry.register_command("/broadcast", [&](std::string text) -> std::string {
        return service.broadcast(std::move(text));
    });
    
    registry.register_command("/history", [&](RoomName room) -> std::string {
        auto* r = service.store().room(room.value);
        if (!r) return "Room not found: " + room.value;
        
        std::string result;
        for (const auto& msg : r->messages()) {
            result += "  " + msg->author() + ": " + msg->render() + "\n";
        }
        return result.empty() ? "No messages" : result;
    });
    
    registry.register_command("/rooms", [&]() -> std::string {
        std::string result = "Available rooms:\n";
        service.store().for_each_room_sorted([&](const std::string& name, const Room& r) {
            result += "  " + name + " (" + std::to_string(r.messages().size()) + " messages)\n";
        });
        return result;
    });

    std::cout << "Chat CLI (stage 10). Engine: " << engine_name << "\n\n";
    
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