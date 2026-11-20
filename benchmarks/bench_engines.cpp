#include "chat_core/i_chat_store.hpp"
#include "chat_core/message.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

using namespace chat;

void bench_engine(const std::string& name, int num_rooms, int num_users, int num_messages) {
    std::cout << "\n=== " << name << " ===\n";
    
    auto engine = make_engine(name);
    
    // Вставка комнат
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_rooms; ++i) {
        engine->create_room("room" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  Create " << num_rooms << " rooms: " << ms << " ms\n";
    
    // Вставка пользователей
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_users; ++i) {
        engine->create_user("user" + std::to_string(i));
    }
    end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  Create " << num_users << " users: " << ms << " ms\n";
    
    // Поиск комнат
    start = std::chrono::high_resolution_clock::now();
    int found = 0;
    for (int i = 0; i < num_rooms; ++i) {
        if (engine->room("room" + std::to_string(i))) ++found;
    }
    end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  Lookup " << num_rooms << " rooms: " << ms << " ms, found=" << found << "\n";
    
    // Добавление сообщений
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_messages; ++i) {
        std::string room = "room" + std::to_string(i % num_rooms);
        engine->add_message(room, make_text_message(
            MessageId{static_cast<uint64_t>(i), Timestamp{}, "user"},
            "user", "message"
        ));
    }
    end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "  Add " << num_messages << " messages: " << ms << " ms\n";
}

int main() {
    bench_engine("std", 1000, 1000, 10000);
    bench_engine("hash", 1000, 1000, 10000);
    
    bench_engine("std", 10000, 10000, 100000);
    bench_engine("hash", 10000, 10000, 100000);
    
    return 0;
}