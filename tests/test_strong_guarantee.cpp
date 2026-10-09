#include "chat_core/chat_store.hpp"
#include "chat_core/errors.hpp"
#include "chat_core/message.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <vector>

using namespace chat;

class ThrowingMessage {
public:
    static int throw_on_copy;
    static int copy_count;

    std::string text;

    ThrowingMessage() = default;
    explicit ThrowingMessage(std::string t) : text(std::move(t)) {}

    ThrowingMessage(const ThrowingMessage& other) : text(other.text) {
        ++copy_count;
        if (throw_on_copy > 0 && copy_count >= throw_on_copy) {
            throw std::runtime_error("Copy failed");
        }
    }

    ThrowingMessage(ThrowingMessage&&) noexcept = default;

    ThrowingMessage& operator=(const ThrowingMessage& other) {
        text = other.text;
        ++copy_count;
        if (throw_on_copy > 0 && copy_count >= throw_on_copy) {
            throw std::runtime_error("Copy failed");
        }
        return *this;
    }

    ThrowingMessage& operator=(ThrowingMessage&&) noexcept = default;

    static void reset() {
        throw_on_copy = 0;
        copy_count = 0;
    }
};

int ThrowingMessage::throw_on_copy = 0;
int ThrowingMessage::copy_count = 0;

TEST(StrongGuarantee, BroadcastRollsBackOnException) {
    ChatStore store;
    store.create_room("room1");
    store.create_room("room2");
    store.create_room("room3");

    store.post("room1", Message{"user", TextBody{"msg1"}, {}});
    store.post("room2", Message{"user", TextBody{"msg2"}, {}});
    store.post("room3", Message{"user", TextBody{"msg3"}, {}});

    auto size_before_1 = store.history("room1")->get().size();
    auto size_before_2 = store.history("room2")->get().size();
    auto size_before_3 = store.history("room3")->get().size();

    ThrowingMessage::reset();
    ThrowingMessage::throw_on_copy = 3;

    std::vector<std::string> rooms = {"room1", "room2", "room3"};

    std::vector<std::string> rooms_with_unknown = {"room1", "room2", "unknown"};

    EXPECT_THROW(
        store.broadcast(rooms_with_unknown, Message{"user", TextBody{"broadcast"}, {}}),
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
    store.broadcast(rooms, Message{"user", TextBody{"broadcast"}, {}});

    EXPECT_EQ(store.history("room1")->get().size(), 1u);
    EXPECT_EQ(store.history("room2")->get().size(), 1u);
}