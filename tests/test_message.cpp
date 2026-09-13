#include "chat_core/chat_store.hpp"
#include "chat_core/message.hpp"
#include "chat_core/spy.hpp"

#include <gtest/gtest.h>

#include <utility>
#include <variant>
#include <vector>

using namespace chat;

TEST(Message, DefaultConstructionIsEmpty) {
    Message m;
    EXPECT_TRUE(m.author.empty());
    ASSERT_TRUE(std::holds_alternative<TextBody>(m.body));
    EXPECT_TRUE(std::get<TextBody>(m.body).text.empty());
}

TEST(Message, TextConstructorCreatesTextBody) {
    Message m{"alice", "hello", {}};
    EXPECT_EQ(m.author, "alice");
    ASSERT_TRUE(std::holds_alternative<TextBody>(m.body));
    EXPECT_EQ(std::get<TextBody>(m.body).text, "hello");
}

TEST(Message, VariantConstructorWorks) {
    Message m{"sys", SystemNotice{"joined"}, {}};
    ASSERT_TRUE(std::holds_alternative<SystemNotice>(m.body));
    EXPECT_EQ(std::get<SystemNotice>(m.body).notice, "joined");
}

TEST(Message, MoveDoesNotInvokeCopy) {
    Spy::reset();
    Message m1{"alice", "hi", {}};
    Spy::reset();

    Message m2 = std::move(m1);

    EXPECT_EQ(Spy::copy_ctor_count, 0u);
    EXPECT_EQ(Spy::move_ctor_count, 1u);
    EXPECT_EQ(m2.author, "alice");
    ASSERT_TRUE(std::holds_alternative<TextBody>(m2.body));
    EXPECT_EQ(std::get<TextBody>(m2.body).text, "hi");
}

TEST(Message, CopyInvokesCopyNotMove) {
    Spy::reset();
    Message m1{"alice", "hi", {}};
    Spy::reset();

    Message m2 = m1;

    EXPECT_EQ(Spy::copy_ctor_count, 1u);
    EXPECT_EQ(Spy::move_ctor_count, 0u);
}

TEST(ChatStore, PostingTemporaryDoesNotCopy) {
    Spy::reset();
    ChatStore store;
    store.create_room("r");
    Spy::reset();

    store.post("r", Message{"a", "b", {}});

    EXPECT_EQ(Spy::copy_ctor_count, 0u);
    EXPECT_GE(Spy::move_ctor_count, 1u);
}

TEST(ChatStore, PostingLvalueCopies) {
    Spy::reset();
    ChatStore store;
    store.create_room("r");
    Message m{"a", "b", {}};
    Spy::reset();

    store.post("r", m);

    EXPECT_EQ(Spy::copy_ctor_count, 1u);
}