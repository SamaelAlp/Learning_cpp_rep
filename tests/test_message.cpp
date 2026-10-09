#include "chat_core/message.hpp"
#include "chat_core/spy.hpp"
#include "chat_core/text_message.hpp"

#include <gtest/gtest.h>

#include <memory>

using namespace chat;

TEST(Message, DefaultConstructionIsEmpty) {
    Message m;
    EXPECT_TRUE(m.author.empty());
    EXPECT_EQ(m.body, nullptr);
}

TEST(Message, TextConstructorCreatesTextMessage) {
    Message m{"alice", "hello", {}};
    EXPECT_EQ(m.author, "alice");
    ASSERT_NE(m.body, nullptr);
    EXPECT_EQ(m.body->to_string(), "hello");
    EXPECT_EQ(m.body->type_name(), "Text");
}

TEST(Message, MoveDoesNotInvokeCopy) {
    Spy::reset();
    Message m1{"alice", "hi", {}};
    Spy::reset();

    Message m2 = std::move(m1);

    EXPECT_EQ(Spy::copy_ctor_count, 0u);
    EXPECT_EQ(Spy::move_ctor_count, 1u);
    EXPECT_EQ(m2.author, "alice");
    EXPECT_EQ(m2.body->to_string(), "hi");
}

TEST(Message, CopyInvokesCopyNotMove) {
    Spy::reset();
    Message m1{"alice", "hi", {}};
    Spy::reset();

    Message m2 = m1;

    EXPECT_EQ(Spy::copy_ctor_count, 1u);
    EXPECT_EQ(Spy::move_ctor_count, 0u);
    EXPECT_EQ(m2.body->to_string(), "hi");
}