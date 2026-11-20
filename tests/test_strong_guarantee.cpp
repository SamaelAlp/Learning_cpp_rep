#include "chat_core/chat_store.hpp"
#include "chat_core/errors.hpp"
#include "chat_core/message.hpp"
#include "chat_core/text_message.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

using namespace chat;

TEST(StrongGuarantee, BroadcastRollsBackOnException) {
    ChatStore store;
    store.create_room("room1");
    store.create_room("room2");
    store.create_room("room3");

    store.post("room1", Message{"user", "msg1", {}});
    store.post("room2", Message{"user", "msg2", {}});
    store.post("room3", Message{"user", "msg3", {}});

    auto size_before_1 = store.history("room1")->get().size();
    auto size_before_2 = store.history("room2")->get().size();
    auto size_before_3 = store.history("room3")->get().size();

    std::vector<std::string> rooms_with_unknown = {"room1", "room2", "unknown"};

    EXPECT_THROW(
        store.broadcast(rooms_with_unknown, Message{"user", "broadcast", {}}),
        UnknownRoomError
    );

    EXPECT_EQ(store.history("room1")->get().size(), size_before_1);
    EXPECT_EQ(store.history("room2")->get().size(), size_before_2);
    EXPECT_EQ(store.history("room3")->get().size(), size_before_3);
}

TEST(StrongGuarantee, BroadcastSucceedsWhenNoException) {
    ChatStore store;
    store.create_room("room1");
    store.create_room("room2");

    std::vector<std::string> rooms = {"room1", "room2"};
    store.broadcast(rooms, Message{"user", "broadcast", {}});

    EXPECT_EQ(store.history("room1")->get().size(), 1u);
    EXPECT_EQ(store.history("room2")->get().size(), 1u);
}