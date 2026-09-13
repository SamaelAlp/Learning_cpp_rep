#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <string>
#include <variant>
#include <vector>

using namespace chat;

TEST(ChatStore, CreateRoomIncreasesCount) {
    ChatStore store;
    EXPECT_EQ(store.room_count(), 0u);
    store.create_room("a");
    EXPECT_EQ(store.room_count(), 1u);
    store.create_room("b");
    EXPECT_EQ(store.room_count(), 2u);
}

TEST(ChatStore, DuplicateCreateIsNoop) {
    ChatStore store;
    store.create_room("a");
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

TEST(ChatStore, PostToUnknownRoomIsIgnored) {
    ChatStore store;
    store.post("ghost", Message{"a", "x", {}});
    EXPECT_EQ(store.room_count(), 0u);
    EXPECT_FALSE(store.history("ghost").has_value());
}

TEST(ChatStore, ForEachRoomVisitsAll) {
    ChatStore store;
    store.create_room("a");
    store.create_room("b");
    store.post("a", Message{"x", "1", {}});
    store.post("b", Message{"y", "2", {}});
    store.post("b", Message{"z", "3", {}});

    std::size_t total = 0;
    std::vector<std::string> names;
    store.for_each_room([&](const std::string& room,
                            const std::vector<Message>& msgs) {
        names.push_back(room);
        total += msgs.size();
    });

    EXPECT_EQ(total, 3u);
    EXPECT_EQ(names.size(), 2u);
}

TEST(ChatStore, MessageAtReturnsCorrectMessage) {
    ChatStore store;
    store.create_room("r");
    store.post("r", Message{"a", "first", {}});
    store.post("r", Message{"b", "second", {}});

    if (auto m = store.message_at("r", 1); m) {
        EXPECT_EQ(m->get().author, "b");
    } else {
        FAIL();
    }
}

TEST(ChatStore, MessageAtReturnsNulloptForBadIndex) {
    ChatStore store;
    store.create_room("r");
    EXPECT_FALSE(store.message_at("r", 0).has_value());
    EXPECT_FALSE(store.message_at("nope", 0).has_value());
}