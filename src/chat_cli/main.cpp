#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"
#include "chat_core/spy.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

using namespace chat;
using namespace std::chrono_literals;

static void demo_vector_behavior() {
    std::cout << "\n=== Звёздочка: поведение std::vector ===\n";

    std::cout << "\n[1] push_back временного Message (rvalue):\n";
    Spy::reset();
    {
        std::vector<Message> v;
        v.reserve(1);
        v.push_back(Message{"a", "hello", {}});
    }
    std::cout << "  copy_ctor=" << Spy::copy_ctor_count
              << "  move_ctor=" << Spy::move_ctor_count << "\n";

    std::cout << "\n[2] push_back переменной-сообщения (lvalue):\n";
    Spy::reset();
    {
        std::vector<Message> v;
        v.reserve(1);
        Message m{"a", "hello", {}};
        Spy::reset();
        v.push_back(m);
    }
    std::cout << "  copy_ctor=" << Spy::copy_ctor_count
              << "  move_ctor=" << Spy::move_ctor_count << "\n";

    std::cout << "\n[3] push_back std::move(m):\n";
    Spy::reset();
    {
        std::vector<Message> v;
        v.reserve(1);
        Message m{"a", "hello", {}};
        Spy::reset();
        v.push_back(std::move(m));
    }
    std::cout << "  copy_ctor=" << Spy::copy_ctor_count
              << "  move_ctor=" << Spy::move_ctor_count << "\n";

    std::cout << "\n[4] Перевыделение вектора (без reserve):\n";
    Spy::reset();
    {
        std::vector<Message> v;
        for (int i = 0; i < 8; ++i) {
            v.push_back(Message{"a", "x", {}});
        }
    }
    std::cout << "  copy_ctor=" << Spy::copy_ctor_count
              << "  move_ctor=" << Spy::move_ctor_count << "\n";

}

int main() {
    ChatStore store;
    store.create_room("general");
    store.create_room("random");

    store.post("general", Message{"alice", "hello everyone", {}});
    store.post("general", Message{"bob",   "hi alice",       {}});
    store.post("random",  Message{"carol", "random thoughts",{}});

    int total_by_ref = 0;
    store.for_each_room([&](const std::string& name, const std::vector<Message>& msgs) {
        std::cout << "room '" << name << "': " << msgs.size() << " messages\n";
        total_by_ref += static_cast<int>(msgs.size());
    });
    std::cout << "Total (by ref):    " << total_by_ref << "\n";

    int total_by_val = 0;
    store.for_each_room([total_by_val](const std::string& name,
                                       const std::vector<Message>& msgs) mutable {
        total_by_val += static_cast<int>(msgs.size());
        std::cout << "room '" << name << "' (inner counter): " << total_by_val << "\n";
    });
    std::cout << "Total (by value):  " << total_by_val << "\n";

    demo_vector_behavior();

    return 0;
}