#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"
#include "chat_core/errors.hpp"

#include <gtest/gtest.h>

#include <string>

using namespace chat;

TEST(ChatStore, CreateRoomIncreasesCount) {
    ChatStore store;
    EXPECT_EQ(store.room_count(), 0u);
    store.create_room("a");
    EXPECT_EQ(store.room_count(), 1u);
}

TEST(ChatStore, RoomReturnsNullptrForUnknown) {
    ChatStore store;
    EXPECT_EQ(store.room("nope"), nullptr);
}

TEST(ChatStore, RoomReturnsPointerForKnown) {
    ChatStore store;
    store.create_room("r");
    EXPECT_NE(store.room("r"), nullptr);
}

TEST(ChatStore, AddMessageToRoom) {
    ChatStore store;
    store.create_room("r");
    auto* r = store.room("r");
    ASSERT_NE(r, nullptr);
    
    r->add_message(make_text_message(
        MessageId{1, Timestamp{}, "alice"},
        "alice",
        "hello"
    ));
    
    EXPECT_EQ(r->messages().size(), 1u);
}

TEST(ChatStore, CreateUserAndFind) {
    ChatStore store;
    auto user = store.create_user("alice");
    ASSERT_NE(user, nullptr);
    EXPECT_EQ(user->nickname(), "alice");
    
    auto found = store.find_user("alice");
    EXPECT_EQ(found, user);
}

TEST(ChatStore, RemoveUser) {
    ChatStore store;
    store.create_user("alice");
    store.remove_user("alice");
    EXPECT_EQ(store.find_user("alice"), nullptr);
}