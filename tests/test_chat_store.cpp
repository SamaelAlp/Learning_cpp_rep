#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <string>
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

TEST(ChatStore, HistoryReturnsNullptrForUnknownRoom) {
    ChatStore store;
    EXPECT_EQ(store.history("nope"), nullptr);
}

TEST(ChatStore, HistoryReturnsMessagesAfterPost) {
    ChatStore store;
    store.create_room("r");
    store.post("r", Message{"a", "hi", {}});
    store.post("r", Message{"b", "yo", {}});

    const auto* h = store.history("r");
    ASSERT_NE(h, nullptr);
    ASSERT_EQ(h->size(), 2u);
    EXPECT_EQ((*h)[0].author, "a");
    EXPECT_EQ((*h)[1].author, "b");
}

TEST(ChatStore, PostToUnknownRoomIsIgnored) {
    ChatStore store;
    store.post("ghost", Message{"a", "x", {}});
    EXPECT_EQ(store.room_count(), 0u);
    EXPECT_EQ(store.history("ghost"), nullptr);
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
    store.for_each_room([&](const std::string& name, const std::vector<Message>& msgs) {
        names.push_back(name);
        total += msgs.size();
    });

    EXPECT_EQ(total, 3u);
    EXPECT_EQ(names.size(), 2u);
}