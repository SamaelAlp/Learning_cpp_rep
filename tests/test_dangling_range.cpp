#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <ranges>
#include <vector>

using namespace chat;

auto get_dangling_view() {
    std::vector<MessagePtr> temp_messages;
    temp_messages.push_back(make_text_message(
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        "hello"
    ));
    temp_messages.push_back(make_text_message(
        MessageId{2, Timestamp{}, "bob"},
        "bob",
        "world"
    ));
    
    // ОПАСНО: temp_messages будет уничтожен при выходе из функции.
    // Возвращаемый view будет содержать висячие ссылки.
    return temp_messages | std::views::filter([](const MessagePtr&){ return true; });
}

TEST(DanglingRange, DemonstratingTheTrap) {
    // auto view = get_dangling_view(); // <-- Раскомментировать для UB / ASan краша
    // for (const auto& msg : view) { ... }
    
    EXPECT_TRUE(true);
}