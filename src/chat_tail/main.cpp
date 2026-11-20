#include <lab/kafka/event_bus.hpp>

#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string topic = "chat.events";
    std::string group = "chat_tail";
    
    if (argc > 1) topic = argv[1];
    if (argc > 2) group = argv[2];
    
    std::cout << "Subscribing to topic '" << topic
              << "' as group '" << group << "'...\n";
    
    try {
        lab::kafka::Consumer consumer({
            .topic = topic,
            .group_id = group
        });
        
        std::cout << "Ready. Waiting for events (Ctrl+C to stop)...\n\n";
        
        while (true) {
            for (const auto& record : consumer.poll(std::chrono::milliseconds(500))) {
                std::cout << "[partition " << record.partition
                          << ", offset " << record.offset << "] "
                          << "key=" << record.key << " | "
                          << record.value << "\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}