#include "chat_core/chat_store.hpp"
#include "chat_core/user.hpp"
#include "chat_core/user_registry.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>

using namespace chat;

TEST(UserRegistry, CreateUser) {
    UserRegistry registry;
    auto user = registry.create_user("alice");

    EXPECT_EQ(user->nickname(), "alice");
    EXPECT_EQ(registry.user_count(), 1u);
}

TEST(UserRegistry, RemoveUser) {
    UserRegistry registry;
    auto user = registry.create_user("alice");
    registry.remove_user("alice");

    EXPECT_EQ(registry.user_count(), 0u);
}

TEST(UserRegistry, WeakPtrBecomesExpired) {
    UserRegistry registry;
    std::weak_ptr<User> weak_user;

    {
        auto user = registry.create_user("alice");
        weak_user = user;

        EXPECT_FALSE(weak_user.expired());
        EXPECT_NE(weak_user.lock(), nullptr);
    }

    registry.remove_user("alice");

    EXPECT_TRUE(weak_user.expired());
    EXPECT_EQ(weak_user.lock(), nullptr);
}

TEST(ChatStore, AddParticipant) {
    UserRegistry registry;
    ChatStore store;

    store.create_room("general");
    auto user = registry.create_user("alice");

    store.add_participant("general", user);

    auto participants = store.get_active_participants("general");
    ASSERT_EQ(participants.size(), 1u);
    EXPECT_EQ(participants[0]->nickname(), "alice");
}

TEST(ChatStore, CleanupExpiredParticipants) {
    UserRegistry registry;
    ChatStore store;

    store.create_room("general");
    auto user2 = registry.create_user("bob");

    {
        auto user1 = registry.create_user("alice");
        store.add_participant("general", user1);
        store.add_participant("general", user2);
    }

    registry.remove_user("alice");

    store.cleanup_expired_participants("general");

    auto participants = store.get_active_participants("general");
    ASSERT_EQ(participants.size(), 1u);
    EXPECT_EQ(participants[0]->nickname(), "bob");
}

TEST(ChatStore, RoomDoesNotKeepUserAlive) {
    UserRegistry registry;
    ChatStore store;

    store.create_room("general");

    std::weak_ptr<User> weak_user;
    {
        auto user = registry.create_user("alice");
        weak_user = user;
        store.add_participant("general", user);
    }

    EXPECT_FALSE(weak_user.expired());

    registry.remove_user("alice");

    EXPECT_TRUE(weak_user.expired());
}