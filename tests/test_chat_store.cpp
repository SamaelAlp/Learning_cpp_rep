#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"
#include "chat_core/text_message.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

using namespace chat;

TEST(ChatStore, CreateRoomIncreasesCount) {
    ChatStore store;
    EXPECT_EQ(store.room_count(), 0u);
    store.create_room("a");
    EXPECT_EQ(store.room_count(), 1u);
}

TEST(ChatStore, HistoryReturnsNulloptForUnknownRoom) {
    ChatStore store;
    EXPECT_FALSE(store.history("nope").has_value());
}

TEST(ChatStore, HistoryReturnsMessagesAfterPost) {
    ChatStore store;
    store.create_room("r");
    store.post("r", Message{"a", "hi", {}});
    store.post("r", Message{"b", "yo", {}});

    if (auto h = store.history("r"); h) {
        const auto& msgs = h->get();
        ASSERT_EQ(msgs.size(), 2u);
        EXPECT_EQ(msgs[0].author, "a");
        EXPECT_EQ(msgs[1].author, "b");
    } else {
        FAIL() << "Expected history to be present";
    }
}

TEST(ChatStore, PostToUnknownRoomThrows) {
    ChatStore store;
    EXPECT_THROW(
        store.post("ghost", Message{"a", "x", {}}),
        UnknownRoomError
    );
}

TEST(ChatStore, BroadcastAddsToAllRooms) {
    ChatStore store;
    store.create_room("r1");
    store.create_room("r2");

    std::vector<std::string> rooms = {"r1", "r2"};
    store.broadcast(rooms, Message{"user", "broadcast", {}});

    EXPECT_EQ(store.history("r1")->get().size(), 1u);
    EXPECT_EQ(store.history("r2")->get().size(), 1u);
}