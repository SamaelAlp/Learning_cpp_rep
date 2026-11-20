#include "std_engine.hpp"
#include "concepts.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

using namespace chat;

TEST(Concepts, NickLikeAcceptsValidTypes) {
    StdEngine store;
    std::string s = "alice";
    std::string_view sv = "bob";
    
    store.create_user(std::string(s));
    store.create_user(std::string(sv));
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