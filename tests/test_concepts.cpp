#include "chat_core/concepts.hpp"
#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

using namespace chat;

TEST(Concepts, NickLikeAcceptsValidTypes) {
    ChatStore store;
    std::string s = "alice";
    std::string_view sv = "bob";
    
    store.create_user(s);
    store.create_user(sv);
    store.create_user("carol");
    
    EXPECT_NE(store.find_user("alice"), nullptr);
    EXPECT_NE(store.find_user("bob"), nullptr);
    EXPECT_NE(store.find_user("carol"), nullptr);
}

TEST(Concepts, MessageIsSerializable) {
    static_assert(Serializable<TextMessage>);
    TextMessage m{{0, {}, ""}, "", ""};
    auto bytes = m.to_bytes();
    static_assert(std::is_same_v<decltype(bytes), std::vector<std::byte>>);
    auto m2 = TextMessage::from_bytes(bytes);
    static_assert(std::is_same_v<decltype(m2), TextMessage>);
}